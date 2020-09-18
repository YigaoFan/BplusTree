#pragma once
#include <filesystem>
#include <vector>
#include <memory>
#include <set>
#include <functional>
#include <utility>
#include "StaticConfig.hpp"
#include "FileCache.hpp"
#include "FileReader.hpp"
#include "FileWriter.hpp"
#include "IInsidePositionOwner.hpp"
#include "StorageAllocator.hpp"
#include "../FriendFuncLibDeclare.hpp"

namespace FuncLib::Store
{
	using ::std::enable_shared_from_this;
	using ::std::forward;
	using ::std::function;
	using ::std::make_shared;
	using ::std::move;
	using ::std::pair;
	using ::std::set;
	using ::std::shared_ptr;
	using ::std::size_t;
	using ::std::vector;
	using ::std::filesystem::path;

	/// 一个路径仅有一个 File 对象，这里的功能大部分是提供给模块外部使用的
	/// 那这里就有点问题了，Btree 内部用这个是什么功能
	/// 这样使用场景是不是就不干净了？
	/// 不一定，因为使用方那里还不完善，给那边先保留着这么大的能力吧
	class File : public enable_shared_from_this<File>
	{
	private:
		static set<File*> Files;

		shared_ptr<path> _filename;
		FileCache _cache;
		function<void()> _unloader;
		StorageAllocator _allocator;
		pos_int _currentPos;
	public:
		static shared_ptr<File> GetFile(path const& filename);
		File(path const& filename, pos_int startPos); // for make_shared use in File class only
		void Flush();

		template <typename T>
		shared_ptr<T> Read(shared_ptr<IInsidePositionOwner> posOwner)
		{
			auto addr = posOwner->Addr();
			if (_cache.HasRead<T>(addr))
			{
				return _cache.Read<T>(addr);
			}
			else
			{
				return PackageThenAddToCache(new T(Read<T>(addr)), posOwner);
			}
		}
		
		// 这样的函数是给 DiskPtr 用的吗？
		// 这里的返回值要把 PositionOwner 也给传出去，因为外面要用这个来调用上面的 Read
		// 这里如何应对 string 这种类型？
		template <typename T>
		pair<shared_ptr<T>, shared_ptr<IInsidePositionOwner>> New(T&& t)
		{
			auto owner = _allocator.Allocate<T>();
			auto p = PackageThenAddToCache(new T(forward(t)), owner);
			return { p, owner };
		}

		template <typename T>
		void Delete(shared_ptr<T> object)
		{
			_cache.Remove(object);
			// TODO 这里需要控制不让 shared_ptr 的析构写入
		}

		/// caller should ensure wake all root element, j
		/// ust like a btree can wake all inner elements, but not other btree.
		void ReallocateContent();

		shared_ptr<path> Path() const;

		~File();
	private:

		/// 使用下面这个的场景是：MiddleNode 包含 Elements 的时候，就不能返回 shared_ptr 了
		/// 而是要用这个了，由 MiddleNode 级别来提供缓存的功能，也就是说要读 Elements 只能通过
		/// MiddleNode 来，这个 Elements 对外是隐形的。如果直接读的话，就错了。
		/// 这个函数的返回值不提供返回值缓存功能
		template <typename T>
		T Read(pos_int start)
		{
			// 触发 读 的唯一一个地方
			auto reader = make_shared<FileReader>(this, start);
			return ByteConverter<T>::ReadOut(reader);
		}

		// 读取的地方的 posOwner 和 New 里面产生的 posOwner 要到 StorageAllocator 里面注册一下
		// 还有这里还要这么写吗，外面不是不通过 t 来获取持久性，而是通过 shared_ptr<File> 来保证这个 t 的持久性
		// 还要再思考。
		// 还有 DiskPos 依赖也不是 File 的所有功能，所以可以考虑剥离一部分功能出来形成一个类，来让 DiskPos 依赖
		template <typename T>
		shared_ptr<T> PackageThenAddToCache(T* ptr, shared_ptr<IInsidePositionOwner> posOwner)
		{
			auto p = shared_ptr<T>(ptr, [filename = this->_filename, posOwner = posOwner](auto p)
			{
				// 触发 写 的唯一一个地方
				auto start = posOwner->Addr();
				auto writer = make_shared<FileWriter>(filename, start);
				ByteConverter<T>::WriteDown(*p, writer);
				delete p;
			});
			_cache.Add<T>(posOwner, p);

			return p;
		}
	};
}
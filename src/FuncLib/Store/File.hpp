#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include <memory>
#include <set>
#include <functional>
#include "../../Basic/Exception.hpp"
#include "StaticConfig.hpp"
#include "FileCache.hpp"
#include "FileReader.hpp"
#include "FileWriter.hpp"
#include "IInsidePositionOwner.hpp"
#include "StorageAllocator.hpp"
#include "../FriendFuncLibDeclare.hpp"

namespace FuncLib::Store
{
	using ::std::byte;
	using ::std::enable_shared_from_this;
	using ::std::forward;
	using ::std::function;
	using ::std::ifstream;
	using ::std::make_shared;
	using ::std::move;
	using ::std::ofstream;
	using ::std::owner_less;
	using ::std::set;
	using ::std::shared_ptr;
	using ::std::size_t;
	using ::std::vector;
	using ::std::weak_ptr;
	using ::std::filesystem::path;

	/// 一个路径仅有一个 File 对象
	class File : public enable_shared_from_this<File>
	{
	private:
		static set<weak_ptr<File>, owner_less<weak_ptr<File>>> Files;

		shared_ptr<path> _filename;
		FileCache _cache;
		function<void()> _unloader = []() {};
		shared_ptr<StorageAllocator> _allocator;
		pos_int _currentPos;
	public:
		static shared_ptr<File> GetFile(path const& filename);
		File(path const& filename, pos_int startPos = 0); // for make_shared use in File class only
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
		template <typename T>
		shared_ptr<T> New(T&& t)
		{
			// auto posOwner = MakePositionOwner<T>(shared_from_this(), _currentPos);
			auto p = AddNewToCache(new T(forward(t)));
			return p;
		}

		template <typename T>
		void Delete(shared_ptr<T> object)
		{
			using ::Basic::InvalidOperationException;
			constexpr auto minRefCount = 3;
			// if (object.use_count() == minRefCount)
			// {
				// log object.use_count();
				_cache.Remove(object);
			// }
			// else
			// {
			// 	throw InvalidOperationException
			// 		("object to delete still has some place using");
			// }
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
			auto reader = make_shared<FileReader>(_filename, start);
			return ByteConverter<T>::ConvertFromByte(reader);
		}

		// 读取的地方的 posOwner 和 New 里面产生的 posOwner 要到 StorageAllocator 里面注册一下
		template <typename T>
		shared_ptr<T> PackageThenAddToCache(T* ptr, shared_ptr<IInsidePositionOwner> posOwner)
		{

			auto pos = posOwner->Addr(); // IInsidePositionOwner 这个类应该是不需要了
			auto p = shared_ptr<T>(ptr, [file = _filename, posOwner = posOwner, handle = _allocator->Register(pos)](auto p)
			{
				// 触发 写 的唯一一个地方
				auto start = handle.GetConcretePosition();
				auto writer = make_shared<FileWriter>(_filename, start);
				// ConvertToByte 名字要改一下，因为这里是直接在里面保存了
				ByteConverter<T>::ConvertToByte(*p, writer);
				delete p;
			});
			_cache.Add<T>(posOwner, p);

			return p;
		}

		// 使用下面这种
		template <typename T>
		shared_ptr<T> AddNewToCache(T* ptr)
		{
			auto p = shared_ptr<T>(ptr, [file = _filename, alloc = _allocator](auto p)
			{
				auto bytes = ByteConverter<T>::ConvertToByte(*p);
				auto pos = alloc->Allocate(bytes.size());
				Write(*file, pos, bytes.begin(), bytes.end());
				delete p;
			});
			_cache.Add<T>(posOwner, p);

			return p;
		}
	};
}
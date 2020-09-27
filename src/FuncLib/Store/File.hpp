#pragma once
#include <filesystem>
#include <memory>
#include <set>
#include <utility>
#include "StaticConfig.hpp"
#include "FileCache.hpp"
#include "FileReader.hpp"
#include "PreWriter.hpp"
#include "StorageAllocator.hpp"
#include "../FriendFuncLibDeclare.hpp"

namespace FuncLib::Store
{
	using ::std::enable_shared_from_this;
	using ::std::forward;
	using ::std::make_shared;
	using ::std::move;
	using ::std::pair;
	using ::std::set;
	using ::std::shared_ptr;
	using ::std::filesystem::path;

	/// 一个路径仅有一个 File 对象，这里的功能大部分是提供给模块外部使用的
	/// 那这里就有点问题了，Btree 内部用这个是什么功能
	/// 这样使用场景是不是就不干净了？指的是内外都用。
	/// 不一定，因为使用方那里还不完善，给那边先保留着这么大的能力吧
	class File : public enable_shared_from_this<File>
	{
	private:
		static set<File*> Files;

		shared_ptr<path> _filename;
		FileCache _cache;
		StorageAllocator _allocator;
	public:
		static shared_ptr<File> GetFile(path const& filename);
		File(unsigned int fileId, path filename); // for make_shared use in File class only
		shared_ptr<path> Path() const;
		~File();

		template <typename T>
		shared_ptr<T> Read(pos_lable posLable)
		{
			if (_cache.HasRead<T>(posLable))
			{
				return _cache.Read<T>(posLable);
			}
			else
			{
				return PackageThenAddToCache(new T(Read<T>(posLable)), posLable);
			}
		}
		
		// 这样的函数是给 DiskPtr 用的吗？
		template <typename T>
		pair<pos_lable, shared_ptr<T>> New(T&& t)
		{
			auto lable = _allocator.AllocatePosLable();
			auto obj = PackageThenAddToCache(new T(forward<T>(t)), lable);
			return { lable, obj };
		}

		template <typename T>
		void Store(pos_lable posLable, shared_ptr<T> const& object)
		{
			// 触发 写 的唯一一个地方
			if (_allocator.Ready(posLable))
			{
				auto oldStart = _allocator.GetConcretePos(posLable);
				if constexpr (ByteConverter<T>::SizeStable)
				{
					auto writer = FileWriter(_filename, oldStart);// 如果这里是子调用 Store，可以合并上层的 writer，只形成一次写入吗？
					ByteConverter<T>::WriteDown(*object, &writer);
				}
				else
				{
					auto writer = PreWriter();// 这个应该只用一次，析构时自动写入，或并入上层 writer
					ByteConverter<T>::WriteDown(*object, &writer);
					auto previousSize = _allocator.GetAllocatedSize(posLable);
					auto newSize = writer.Size();
					if (previousSize < newSize))
					{
						auto newStart = _allocator.ResizeSpaceTo(posLable, newSize);
						writer.StartPos(newStart);
					}
					else
					{
						writer.StartPos(oldStart);
					}
				}
			}
			else
			{
				auto writer = PreWriter(); // 这个应该只用一次（减少复杂性），析构时自动写入，或并入上层 writer
				ByteConverter<T>::WriteDown(*object, &writer);
				auto size = writer.Size();
				auto start = _allocator.GiveSpaceTo(posLable);
				writer.StartPos(start);
			}
			
			// auto internalWriter = make_shared<FileWriter>(filename, start);
			// 可能需要 assert 这里的 start 和 writer 的当前地址要一样，有的情况下可能不一样也是对的
			// 要基于位置都是偏移的抽象的基础去工作，感觉有点复杂了可能，之后再想

			if constexpr (!ByteConverter<T>::SizeStable)// string 的 ByteConverter 那里可能要改成 SizeStable，map 也是，加一个指针进去，那不 SizeStable 的部分在哪里？
			{
				auto previousSize = _allocator.GetAllocatedSize();
				auto newSize = internalWriter->BufferSize();
				if ( previousSize < newSize))
				{
					alloc->Reallocate(posLable, newSize);
					auto newPos = _allocator.GetConcretePos(posLable);
					internalWriter->SetStartPos(newPos);
					// 然后这里需要把这个 internal writer 给 append 到 writer 上
					// 那么这样的话，就要有 buffer 了，而且 buffer 里的每一块要存具体地址，以隔离别的存储地址的影响
					// writer->Obtain(move(internalWriter));
					// 大概代码类似上面这样
				}
			}
		}

		template <typename T>
		void Delete(pos_lable posLable, shared_ptr<T> object) // 这个模仿 delete 这个接口，但暂不处理 object
		{
			_cache.Remove<T>(posLable);
			_allocator.DeallocatePosLable(posLable);
		}

	private:
		template <typename T>
		T Read(pos_lable posLable)
		{
			// 触发 读 的唯一一个地方
			auto start = _allocator.GetConcretePos(posLable);
			auto reader = FileReader(this, _filename, start);
			return ByteConverter<T>::ReadOut(&reader);
		}

		// 还有这里还要这么写吗，外面不是不通过 t 来获取持久性，而是通过 shared_ptr<File> 来保证这个 t 的持久性
		// 还有 DiskPos 依赖也不是 File 的所有功能，所以可以考虑剥离一部分功能出来形成一个类，来让 DiskPos 依赖
		template <typename T>
		shared_ptr<T> PackageThenAddToCache(T* ptr, pos_lable posLable)
		{
			auto obj = shared_ptr<T>(ptr);
			_cache.Add<T>(posLable, obj);
			return obj;
		}
	};
}
#pragma once
#include <filesystem>
#include <memory>
#include <set>
#include <utility>
#include "../../Basic/TypeTrait.hpp"
#include "StaticConfig.hpp"
#include "FileCache.hpp"
#include "FileReader.hpp"
#include "FileWriter.hpp"
#include "PreWriter.hpp"
#include "StorageAllocator.hpp"
#include "../FriendFuncLibDeclare.hpp"// 因为 DiskPos 里面有功能依赖 File，所以这里只能 ByteConverter 的声明

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
		set<pos_lable> _toDeallocateLables;// 之后可以基于这个调整文件大小
	public:
		static shared_ptr<File> GetFile(path const& filename);
		File(unsigned int fileId, path filename); // for make_shared use in File class only
		shared_ptr<path> Path() const;
		~File();

		template <typename T>
		shared_ptr<T> Read(pos_lable posLable)
		{
			if (_cache.Cached<T>(posLable))
			{
				return _cache.Read<T>(posLable);
			}
			else
			{
				return AddToCache(ReadOn<T>(posLable), posLable);
			}
		}

		template <typename T>
		pair<pos_lable, shared_ptr<T>> New(T t)
		{
			auto lable = _allocator.AllocatePosLable();
			auto obj = AddToCache(move(t), lable);
			return { lable, obj };
		}

		template <typename T>
		pair<pos_lable, shared_ptr<T>> New(shared_ptr<T> t)
		{
			auto lable = _allocator.AllocatePosLable();
			auto obj = AddToCache(move(t), lable);
			return { lable, obj };
		}

		template <typename T>
		void Store(pos_lable posLable, shared_ptr<T> const& object)
		{
			_toDeallocateLables.erase(posLable);

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
					auto start = oldStart;
					if (previousSize < newSize)
					{
						start = _allocator.ResizeSpaceTo(posLable, newSize);
					}

					FileWriter(move(writer), _filename, start);
				}
			}
			else
			{
				auto writer = PreWriter(); // 这个应该只用一次（减少复杂性），析构时自动写入，或并入上层 writer
				ByteConverter<T>::WriteDown(*object, &writer);
				auto size = writer.Size();
				auto start = _allocator.GiveSpaceTo(posLable, size);
				FileWriter(move(writer), _filename, start);
			}
			
			// 可能需要 assert 这里的 start 和 writer 的当前地址要一样，有的情况下可能不一样也是对的
			// 要基于位置都是偏移的抽象的基础去工作，感觉有点复杂了可能，之后再想
		}

		template <typename T>
		void Delete(pos_lable posLable, shared_ptr<T> object) // 这个模仿 delete 这个接口，但暂不处理 object
		{
			_toDeallocateLables.erase(posLable);

			_cache.Remove<T>(posLable);
			_allocator.DeallocatePosLable(posLable);
		}

	private:
		template <typename T>
		auto ReadOn(pos_lable posLable)
		{
			// 触发 读 的唯一一个地方
			auto start = _allocator.GetConcretePos(posLable);
			auto reader = FileReader(this, _filename, start);
			return ByteConverter<T>::ReadOut(&reader);
		}

		// 还有 DiskPos 依赖也不是 File 的所有功能，所以可以考虑剥离一部分功能出来形成一个类，来让 DiskPos 依赖
		template <typename T>
		shared_ptr<T> AddToCache(T t, pos_lable posLable)
		{
			_toDeallocateLables.insert(posLable);
			shared_ptr<T> obj = make_shared<T>(move(t));
			_cache.Add<T>(posLable, obj);
			return obj;
		}

		template <typename T>
		shared_ptr<T> AddToCache(shared_ptr<T> obj, pos_lable posLable)
		{
			_toDeallocateLables.insert(posLable);
			_cache.Add<T>(posLable, obj);
			return obj;
		}
	};
}
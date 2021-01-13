#pragma once
#include <filesystem>
#include <memory>
#include <set>
#include <utility>
#include "../../Basic/TypeTrait.hpp"
#include "StaticConfig.hpp"
#include "FileCache.hpp"
#include "FileReader.hpp"
#include "ObjectBytes.hpp"
#include "StorageAllocator.hpp"
// 这里用到 ByteConverter，但因为 DiskPos 里面有功能依赖 File，所以这里只能声明 ByteConverter
#include "../Persistence/FriendFuncLibDeclare.hpp"
#include "ObjectRelation/ObjectRelationTree.hpp"
#include "ObjectRelation/ReadStateLabelNode.hpp"
#include "CacheSearchRoutine.hpp"
#include "FakeObjectBytes.hpp"

namespace FuncLib::Store
{
	using namespace FuncLib::Store::ObjectRelation;
	using FuncLib::Persistence::ByteConverter;
	using FuncLib::Persistence::DiskPos;
	using FuncLib::Persistence::Switch;
	using FuncLib::Persistence::TakeWithDiskPos;
	using ::std::enable_shared_from_this;
	using ::std::fstream;
	using ::std::is_base_of_v;
	using ::std::make_shared;
	using ::std::move;
	using ::std::ofstream;
	using ::std::pair;
	using ::std::set;
	using ::std::shared_ptr;
	using ::std::filesystem::exists;
	using ::std::filesystem::path;

	/// 一个路径仅有一个 File 对象，这里的功能大部分是提供给模块外部使用的
	/// 那这里就有点问题了，Btree 内部用这个是什么功能
	/// 这样使用场景是不是就不干净了？指的是内外都用。
	/// 不一定，因为使用方那里还不完善，给那边先保留着这么大的能力吧
	class File : public enable_shared_from_this<File>
	{
	private:
		static inline unsigned int FileCount = 0;
		static constexpr pos_int MetadataSize = 2048; // Byte，这个应该属于 data member，因为后期会改

		shared_ptr<path> _filename;
		FileCache _cache;
		StorageAllocator _allocator;
		set<pos_label> _notStoredLabels;// 之后可以基于这个调整文件大小，这个是为了对象从 New 到 Store 保证的
		ObjectRelationTree _objRelationTree;
	public:
		static shared_ptr<File> GetFile(path const& filename);
		/// below for make_shared use in File class only
		File(unsigned int fileId, shared_ptr<path> filename, StorageAllocator allocator, ObjectRelationTree relationTree);
		File(File&& that) noexcept = delete;
		File(File const& that) = delete;

		shared_ptr<path> Path() const;
		~File();

		template <typename T>
		shared_ptr<T> Read(pos_label posLabel)
		{
			using SearchRoutine = typename GenerateSearchRoutine<T>::Result;
			auto obj = Search<T, SearchRoutine>(posLabel);
			return obj;
		}

		/// New 的时候要用本来的类型，而不要用动态类型
		template <typename T>
		auto New(T t)
		{
			auto label = _allocator.AllocatePosLabel();
			_notStoredLabels.insert(label);
			// SetItUp 针对 shared_ptr 有个特化，要注意下
			auto obj = SetItUp(move(t), label);

			return pair{ label, obj };
		}

		/// 使用这个方法进行存储只能是最外层的对象，比如用在 OuterDiskPtr 这样
		template <typename T>
		void Store(pos_label posLabel, shared_ptr<T> const& object)
		{
			_notStoredLabels.erase(posLabel);

			auto fs = MakeFileStream(_filename.get());
			auto allocate = [&](ObjectBytes* bytes)
			{
				auto size = bytes->Size();
				_allocator.GiveSpaceTo(bytes->Label(), size);
			};

			auto resize = [&](ObjectBytes* bytes)
			{
				_allocator.ResizeSpaceTo(bytes->Label(), bytes->Size());
			};

			auto write = [&](ObjectBytes* bytes)
			{
				auto start = _allocator.GetConcretePos(bytes->Label());
				bytes->WriteIn(&fs, start + MetadataSize);
			};

			// 这里的 SizeStable 不考虑指针指向的对象，牵涉到 ByteConverter<DiskPtr> 和这下面的 if
			WriteQueue toWrites;
			AllocateSpaceQueue toAllocates;
			ResizeSpaceQueue toResize;
			ObjectBytes bytes{ posLabel, &toWrites, &toAllocates, &toResize };
			ProcessStore(posLabel, object, &bytes);

			// Ensure file exist before write
			if (!exists(*_filename)) { ofstream f(*_filename); }

			toResize | resize | write; // Resize first, then allocate. Below allocates can reuse place.
			toAllocates | allocate | write;
			toWrites | write;

			auto readStateNode = ReadStateLabelNode::ConsNodeWith(&bytes);
			_objRelationTree.UpdateWith(move(readStateNode));
		}

		/// Precondition: object is not null
		template <typename T>
		void StoreInner(pos_label posLabel, shared_ptr<T> const& object, ObjectBytes* writer)
		{
			_notStoredLabels.erase(posLabel);
			ProcessStore(posLabel, object, writer);
		}

		/// Precondition: object is not null
		template <typename T>
		void StoreInner(pos_label posLabel, shared_ptr<T> const& object, FakeObjectBytes* writer)
		{
			ProcessFakeStore(posLabel, object, writer);
		}

		template <typename T>
		void Delete(pos_label posLabel, shared_ptr<T> object) // 这个模仿 delete 这个接口，但暂不处理 object
		{
			FakeObjectBytes writer{ posLabel };
			ProcessFakeStore(posLabel, object, &writer);

			auto readStateNode = ReadStateLabelNode::ConsNodeWith(&writer);
			_objRelationTree.Free(move(readStateNode));
			_cache.Remove<T>(posLabel);
		}

		template <typename T>
		void RegisterSetter(pos_label posLable, function<void(T*)> setter)
		{
			_cache.RegisterSetter(posLable, move(setter));
		}

	private:
		template <typename Des, typename TypeList>
		shared_ptr<Des> Search(pos_label label)
		{
			using T = typename TypeList::Current;
			if (_cache.Cached<T>(label))
			{
				return _cache.Read<T>(label);
			}

			if constexpr (TypeList::IsLast)
			{
				return SetItUp(ReadOn<T>(label), label);
			}
			else
			{
				return Search<Des, typename TypeList::Remain>(label);
			}
		}

		template <typename T>
		auto ReadOn(pos_label posLabel)
		{
			// 触发 读 的唯一一个地方
			auto start = _allocator.GetConcretePos(posLabel);
			auto reader = FileReader::MakeReader(this, *_filename, start);
			return ByteConverter<T>::ReadOut(&reader);
		}

		template <typename T>
		void SetDiskPosIfEnable(T* obj, pos_label label)
		{
			if constexpr (is_base_of_v<TakeWithDiskPos<T, Switch::Enable>, T>)
			{
				DiskPos<T> pos(this, label);
				TakeWithDiskPos<T, Switch::Enable>::SetDiskPos(obj, move(pos));
			}
		}

		// 还有 DiskPos 依赖也不是 File 的所有功能，所以可以考虑剥离一部分功能出来形成一个类，来让 DiskPos 依赖
		// 或者让 DiskPos 中依赖 File 的部分作为友元
		template <typename T>
		shared_ptr<T> SetItUp(T t, pos_label posLabel)
		{
			shared_ptr<T> obj = make_shared<T>(move(t));
			return SetItUp(obj, posLabel);
		}

		template <typename T>
		shared_ptr<T> SetItUp(shared_ptr<T> obj, pos_label posLabel)
		{
			_cache.Add<T>(posLabel, obj);
			SetDiskPosIfEnable(obj.get(), posLabel);
			return obj;
		}

		template <typename T>
		void ProcessStore(pos_label posLabel, shared_ptr<T> const& object, ObjectBytes* bytes)
		{
			ByteConverter<T>::WriteDown(*object, bytes);// 这里把 bytes 准备好，这里的 bytes 都是和地址无关的

			// 决定下一步去向
			if (_allocator.Ready(posLabel))
			{
				if constexpr (not ByteConverter<T>::SizeStable)
				{
					auto previousSize = _allocator.GetAllocatedSize(posLabel);
					auto newSize = bytes->Size();
					if (previousSize < newSize)
					{
						// 加入重分配区
						bytes->ToResizes->Add(bytes);
						return;
					}
				}

				// 加入待写区
				bytes->ToWrites->Add(bytes);
			}
			else
			{
				// 加入待分配区
				bytes->ToAllocates->Add(bytes);
			}
		}

		static fstream MakeFileStream(path const* filename);

		template <typename T>
		void ProcessFakeStore(pos_label posLabel, shared_ptr<T> const& object, FakeObjectBytes* writer)
		{
			ByteConverter<T>::WriteDown(*object, writer);
		}
	};
}
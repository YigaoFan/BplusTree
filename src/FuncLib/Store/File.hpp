#pragma once
#include <set>
#include <memory>
#include <utility>
#include <filesystem>
#include <type_traits>
#include "../../Basic/TypeTrait.hpp"
#include "StaticConfig.hpp"
#include "FileCache.hpp"
#include "FileReader.hpp"
#include "ObjectBytes.hpp"
#include "ObjectBytesQueue.hpp"
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
	using ::std::pair;
	using ::std::remove_const_t;
	using ::std::remove_reference_t;
	using ::std::set;
	using ::std::shared_ptr;
	using ::std::filesystem::path;

	class File : public enable_shared_from_this<File>
	{
	private:
		friend struct FuncLib::Persistence::ByteConverter<File, false>;
		static inline unsigned int FileCount = 0;

		pos_int _metadataSize; // Byte
		shared_ptr<path> _filename;
		FileCache _cache;
		StorageAllocator _allocator;
		set<pos_label> _notStoredLabels;// 之后可以基于这个调整文件大小，这个是为了对象从 New 到 Store 保证的
		ObjectRelationTree _objRelationTree;
	public:
		static shared_ptr<File> GetFile(path const& filename);
		/// below for make_shared use in File class only
		File(pos_int metadataSize, FileCache cache, shared_ptr<path> filename, StorageAllocator allocator, ObjectRelationTree relationTree);
		File(File&& that) noexcept = delete;
		File(File const& that) = delete;
		~File();

		template <typename T>
		bool HasRead(pos_label label) const
		{
			using SearchRoutine = typename Cons<T, typename GenerateOtherSearchRoutine<T>::Result>::Result;
			return HasCached<SearchRoutine>(label);
		}

		template <typename T>
		shared_ptr<T> Read(pos_label posLabel)
		{
			using SearchRoutine = typename GenerateOtherSearchRoutine<T>::Result;
			auto obj = Search<T, SearchRoutine>(posLabel);
			return obj;
		}

		/// New 的时候要用本来的类型，而不要用动态类型
		template <typename T>
		auto New(T&& t)
		{
			auto label = _allocator.AllocatePosLabel();
			_notStoredLabels.insert(label);
			// SetItUp 针对 shared_ptr 有个特化，要注意下
			auto obj = SetItUp(forward<T>(t), label);

			return pair{ label, obj };
		}

		/// New with specified pos_label. If this posLabel is used, exception will be thrown
		template <typename T>
		auto New(pos_label posLabel, T&& t)
		{
			_allocator.AllocateSpecifiedLabel(posLabel);
			_notStoredLabels.insert(posLabel);
			// SetItUp 针对 shared_ptr 有个特化，要注意下
			auto obj = SetItUp(forward<T>(t), posLabel);

			return pair{ posLabel, obj };
		}

		/// 使用这个方法进行存储只能是最外层的对象，比如用在 OuterDiskPtr 这样
		template <typename T>
		void Store(pos_label posLabel, shared_ptr<T> const& object)
		{
			_notStoredLabels.erase(posLabel);
			CreateIfNotExist(_filename.get());

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
				bytes->WriteIn(&fs, start + _metadataSize);
			};

			// 这里的 SizeStable 不考虑指针指向的对象，牵涉到 ByteConverter<DiskPtr> 和这下面的 if
			WriteQueue toWrites;
			AllocateSpaceQueue toAllocates;
			ResizeSpaceQueue toResize;
			ObjectBytes bytes{ posLabel, &toWrites, &toAllocates, &toResize };
			ProcessStore(posLabel, object, &bytes);

			toResize > resize > write; // Resize first, then allocate. Below allocates can reuse place.
			toAllocates > allocate > write;
			toWrites > write;
			fs.flush();

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

			using SearchRoutine = typename Cons<T, typename GenerateOtherSearchRoutine<T>::Result>::Result;
			TryRemoveCache<SearchRoutine>(posLabel);
		}

		/// In File, Fake Store is for delete
		template <typename T>
		void Delete(pos_label posLabel, shared_ptr<T> object) // 这个模仿 delete 这个接口，但暂不处理 object
		{
			FakeObjectBytes writer{ posLabel };
			ProcessFakeStore(posLabel, object, &writer);

			auto readStateNode = ReadStateLabelNode::ConsNodeWith(&writer);
			_objRelationTree.Free(move(readStateNode));

			using SearchRoutine = typename Cons<T, typename GenerateOtherSearchRoutine<T>::Result>::Result;
			TryRemoveCache<SearchRoutine>(posLabel);
		}

		template <typename T>
		void RegisterSetter(pos_label posLable, function<void(T*)> setter)
		{
			_cache.RegisterSetter(posLable, move(setter));
		}

	private:
		template <typename SearchTypeList>
		void TryRemoveCache(pos_label label)
		{
			if constexpr (SearchTypeList::IsNull)
			{
				return;
			}
			else
			{
				using T = typename SearchTypeList::Current;
				if (_cache.Cached<T>(label))
				{
					return _cache.Remove<T>(label);
				}

				return TryRemoveCache<typename SearchTypeList::Remain>(label);
			}
		}

		template <typename SearchTypeList>
		bool HasCached(pos_label label) const
		{
			if constexpr (SearchTypeList::IsNull)
			{
				return false;
			}
			else
			{
				using T = typename SearchTypeList::Current;
				if (_cache.Cached<T>(label))
				{
					return true;
				}

				return HasCached<typename SearchTypeList::Remain>(label);
			}
		}

		template <typename Des, typename OtherSearchTypeList, bool FirstCall=true>
		shared_ptr<Des> Search(pos_label label)
		{
			if constexpr (FirstCall)
			{
				using T = Des;
				if (_cache.Cached<T>(label))
				{
					return _cache.Read<T>(label);
				}
			}

			if constexpr (OtherSearchTypeList::IsNull)
			{
				return SetItUp(ReadOn<Des>(label), label);
			}
			else
			{
				using T = typename OtherSearchTypeList::Current;
				if (_cache.Cached<T>(label))
				{
					return static_pointer_cast<Des>(_cache.Read<T>(label));
				}

				return Search<Des, typename OtherSearchTypeList::Remain, false>(label);
			}
		}

		template <typename T>
		auto ReadOn(pos_label posLabel)
		{
			// 触发 读 的唯一一个地方
			auto start = _allocator.GetConcretePos(posLabel);
			// printf("read from %lu\n", start + _metadataSize);
			auto reader = FileReader::MakeReader(this, *_filename, start + _metadataSize);
			return ByteConverter<T>::ReadOut(&reader);
		}

		template <typename Object>
		void SetDiskPosIfEnable(Object* obj, pos_label label)
		{
			if constexpr (not TryGetTakeWithDiskPosDestType<Object>::IsNull)
			{
				using T = typename TryGetTakeWithDiskPosDestType<Object>::Result;
				DiskPos<T> pos(this, label);
				TakeWithDiskPos<T, Switch::Enable>::SetDiskPos(obj, move(pos));
			}
		}

		// 还有 DiskPos 依赖也不是 File 的所有功能，所以可以考虑剥离一部分功能出来形成一个类，来让 DiskPos 依赖
		// 或者让 DiskPos 中依赖 File 的部分作为友元
		/// Precondition: label not cached
		template <typename T>
		auto SetItUp(T&& t, pos_label posLabel) -> shared_ptr<remove_const_t<remove_reference_t<T>>>
		{
			using ElementType = remove_const_t<remove_reference_t<T>>;// 顶层 const 和底层 const 的问题？
			auto obj = make_shared<ElementType>(forward<T>(t));
			return SetItUp(obj, posLabel);
		}

		/// Precondition: label not cached
		template <typename T>
		shared_ptr<T> SetItUp(shared_ptr<T> obj, pos_label posLabel)
		{
			ClearCacheRelatedTypeSetter<typename GenerateOtherSearchRoutine<T>::Result>(posLabel, obj.get());
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
						// printf("%s add to Resizes label %d size %lu\n", typeid(T).name(), posLabel, bytes->Size());
						bytes->ToResizes->Add(move(*bytes)); // move bytes to queue
						return;
					}
				}

				// 加入待写区
				// printf("%s add to Writes label %d size %lu\n", typeid(T).name(), posLabel, bytes->Size());
				bytes->ToWrites->Add(move(*bytes));
			}
			else
			{
				// 加入待分配区
				// printf("%s add to Allocates label %d size %lu\n", typeid(T).name(), posLabel, bytes->Size());
				bytes->ToAllocates->Add(move(*bytes));
			}
		}

		static fstream MakeFileStream(path const* filename);
		static void CreateIfNotExist(path const* filename);
		template <typename T>
		void ProcessFakeStore(pos_label posLabel, shared_ptr<T> const& object, FakeObjectBytes* writer)
		{
			ByteConverter<T>::WriteDown(*object, writer);
		}

		template <typename RelatedTypeList, typename Object>
		void ClearCacheRelatedTypeSetter(pos_label label, Object* object)
		{
			if constexpr (not RelatedTypeList::IsNull)
			{
				using T = typename RelatedTypeList::Current;
				if (_cache.HasSetter<T>(label))
				{
					_cache.Set(label, static_cast<T*>(object));
				}
				ClearCacheRelatedTypeSetter<typename RelatedTypeList::Remain>(label, object);
			}

		}
	};
}
#pragma once
#include <string>
#include <memory>
#include <type_traits>
#include <vector>
#include <functional>
#include "DiskPos.hpp"
#include "CurrentFile.hpp"
#include "../Btree/Btree.hpp"
#include "../Basic/TypeTrait.hpp"

namespace FuncLib
{
	using ::std::string;
	using ::std::vector;
	using ::std::function;
	using ::std::shared_ptr;
	using ::std::move;
	using ::std::enable_if_t;
	using ::std::remove_cvref_t;
	using ::Collections::Btree;
	using ::Collections::order_int;
	using ::Basic::IsSpecialization;

	// Btree should have a type para to pass pointer type
	// 要考虑全部持久化后，以 NodeBase 类型去构造内存里的 MiddleNode 和 LeafNode
	// 所以要支持某种转发机制，让 DiskPtr<NodeBase> 转发到 DiskPtr<MiddleNode> 或者 DiskPtr<LeafNode>


	template <typename T>
	class DiskPtrBase
	{
	protected:
		shared_ptr<T> tPtr = nullptr; // TODO Cache
		DiskPos<T> _pos;
		vector<function<void(T*)>> _contentSetter;

	public:
		DiskPtrBase(DiskPos<T> pos) : _pos(pos)
		{ }

		void RegisterSetter(function<void(T*)> callback)
		{
			_contentSetter.push_back(move(callback));
		}

		T* operator-> ()
		{
			if (tPtr == nullptr)
			{
				tPtr = _pos.ReadObject();
			}
			
			if (!_contentSetter.empty())
			{
				for (auto& f : _contentSetter)
				{
					f(tPtr.get());
				}

				_contentSetter.clear();
			}

			return tPtr;
		}

		~DiskPtrBase()
		{
			_pos.WriteObject();
		}
	};

	template <typename T>
	class WeakDiskPtr : public DiskPtrBase<T>
	{
	private:
		friend struct DiskDataConverter<WeakDiskPtr>;
		using Base = DiskPtrBase<T>;

	public:
		using Base::Base;
	};

	template <typename T>
	class DiskPtr : public DiskPtrBase<T>
	{
	private:
		friend struct DiskDataConverter<DiskPtr>;
	public:
		using Base = DiskPtrBase<T>;

	public:
		using Base::Base;

		// this ptr can destory data on disk
		DiskPtr(DiskPtr& const) = delete;
		WeakDiskPtr<T> GetWeakPtr()
		{

		}
	};


	// For Node
	/*template <typename T>
	class DiskPtr
	{
		void WriteToDisk();

	};*/
	// Btree 中用到了几种数据？
	// 一种是实体，比如 Key，Value 最终在叶子节点存的都是实体，Node 之类应该是也是
	// 一种是存的指针，比如 unique_ptr，也就是说指针本身也要持久化
	//template <typename T>
	//class DiskPtr<DiskPtr<T>> // 即 DiskPtr 如何存在硬盘上
	//{
	//	// 这里想到了一个问题，如何防止一个地方被重复读取
	//	// 即硬盘里的都是原始的，需要还原的，但每个都还原，就会重复了
	//	// 用缓存？
	//};

	// Update content to disk
	// Or register update event in allocator, 然后集中更新
	// Use DiskPos to update

	template <typename Key, typename Value, order_int BtreeOrder>
	class DiskPtr<Btree<BtreeOrder, Key, Value, DiskPtr>> : public DiskPtrBase<Btree<BtreeOrder, Key, Value>>
	// TODO change Btree parameter order
	{
	private:
		using Content = Btree<BtreeOrder, Key, Value, DiskPtr>;
		using Base = DiskPtrBase<Btree<BtreeOrder, Key, Value>>;
	public:
		static DiskPtr ReadBtreeFrom(File file, size_t startOffset = 0)
		{
			return { {make_shared<File>(move(file)), startOffset} };
		}

	private:
		DiskPtr(DiskPos<Content> pos) : Base(move(pos))
		{ }
	};

	template <typename Key, typename Value, order_int BtreeOrder>
	using DiskBtree = DiskPtr<Btree<BtreeOrder, Key, Value, DiskPtr>>;

	template <typename Ptr>
	struct GetContentType;

	template <typename T>
	struct GetContentType<unique_ptr<T>>
	{
		using Type = T;
	};

	template <typename T>
	struct GetContentType<DiskPtr<T>>
	{
		using Type = T;
	};

	template <typename T>
	struct GetContentType<T*>
	{
		using Type = T;
	};

	template <typename Ptr>
	void SetProperty(Ptr& t, function<void(typename GetContentType<Ptr>::Type*)> setter)
	{
		using Type = remove_cvref_t<Ptr>;
		if constexpr (IsSpecialization<Type, unique_ptr>::value)
		{
			setter(t.get());
		}
		else
		{
			static_assert(IsSpecialization<Type, DiskPtr>::value);
			t.RegisterSetter(setter);
		}
	}
}
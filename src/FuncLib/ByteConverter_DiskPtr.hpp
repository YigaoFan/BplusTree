#pragma once
#include <type_traits>
#include <memory>
#include "../Btree/Btree.hpp"
#include "../Btree/NodeBase.hpp"
#include "../Btree/MiddleNode.hpp"
#include "../Btree/LeafNode.hpp"

namespace FuncLib
{
	using ::Collections::Btree;
	using ::Collections::LeafNode;
	using ::Collections::MiddleNode;
	using ::Collections::NodeBase;
	using ::Collections::order_int;
	using ::std::declval;
	using ::std::make_shared;
	using ::std::move;
	using ::std::shared_ptr;

	template <typename T>
	struct ByteConverter<UniqueDiskPtr<T>, false>
	{
		using ThisType = UniqueDiskPtr<T>;
		using DataMemberType = decltype(declval<ThisType>()._pos);
		static constexpr bool SizeStable = All<GetSizeStable, DataMemberType, T>::Result;

		static void WriteDown(ThisType const& p, shared_ptr<FileWriter> writer)
		{
			if (p._tPtr != nullptr)
			{
				using ContentType = decltype(*(p._tPtr));
				ByteConverter<ContentType>::WriteDown(*(p._tPtr), writer);
				// 根据这里保存到的位置来决定下面 _pos 的内容
			}
			ByteConverter<DataMemberType>::WriteDown(p._pos, writer);
			// 这里也要考虑位置是否变化的问题，来优化
		}

		static ThisType ReadOut(shared_ptr<FileReader> reader)
		{
			return ByteConverter<DataMemberType>::ReadOut(reader);
		}
	};

	template <typename T>
	struct ByteConverter<SharedDiskPtr<T>, false>
	{
		using ThisType = SharedDiskPtr<T>;
		using DataMemberType = decltype(declval<ThisType>()._pos);
		static constexpr bool SizeStable = All<GetSizeStable, DataMemberType, T>::Result;

		static void WriteDown(ThisType const& p, shared_ptr<FileWriter> writer)
		{
			if (p._tPtr != nullptr)
			{
				using ContentType = decltype(*(p._tPtr));
				ByteConverter<ContentType>::WriteDown(*(p._tPtr), writer);
			}
			ByteConverter<DataMemberType>::WriteDown(p._pos, writer);
		}

		static ThisType ReadOut(shared_ptr<FileReader> reader)
		{
			return ByteConverter<DataMemberType>::ReadOut(reader);
		}
	};

	template <typename Key, typename Value, order_int Order>
	struct ByteConverter<Btree<Order, Key, Value, UniqueDiskPtr>, false>
	{
		using ThisType = Btree<Order, Key, Value, UniqueDiskPtr>;
		using DataMemberType0 = decltype(declval<ThisType>()._keyCount);
		using DataMemberType1 = decltype(declval<ThisType>()._root);
		static constexpr bool SizeStable = All<GetSizeStable, DataMemberType0, DataMemberType1>::Result;

		static void WriteDown(ThisType const& t, shared_ptr<FileWriter> writer)
		{
			ByteConverter<DataMemberType0>::WriteDown(t._keyCount, writer);
			ByteConverter<DataMemberType1>::WriteDown(t._root, writer);
		}

		static ThisType ReadOut(shared_ptr<FileReader> reader)
		{
			auto member0 = ByteConverter<DataMemberType0>::ReadOut(reader);
			auto member1 = ByteConverter<DataMemberType1>::ReadOut(reader);
			return { member0, move(member1) };
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<MiddleNode<Key, Value, Count, UniqueDiskPtr>, false>
	{
		using ThisType = MiddleNode<Key, Value, Count, UniqueDiskPtr>;
		using DataMemberType = decltype(declval<ThisType>()._elements);
		static constexpr bool SizeStable = All<GetSizeStable, DataMemberType>::Result;

		static void WriteDown(ThisType const& t, shared_ptr<FileWriter> writer)
		{
			ByteConverter<DataMemberType>::WriteDown(t._elements, writer);
		}

		static ThisType ReadOut(shared_ptr<FileReader> reader)
		{
			auto elements = ByteConverter<DataMemberType>::ReadOut(reader);
			return { move(elements) };// TODO provide LeafNode previous, next and callback inner
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<LeafNode<Key, Value, Count, UniqueDiskPtr>, false>
	{
		using ThisType = LeafNode<Key, Value, Count, UniqueDiskPtr>;
		using DataMemberType = decltype(declval<ThisType>()._elements);
		static constexpr bool SizeStable = All<GetSizeStable, DataMemberType>::Result;

		static void WriteDown(ThisType const& t, shared_ptr<FileWriter> writer)
		{
			ByteConverter<DataMemberType>::WriteDown(t._elements, writer);
		}

		static ThisType ReadOut(shared_ptr<FileReader> reader)
		{
			auto elements = ByteConverter<DataMemberType>::ReadOut(reader);
			return { move(elements) };
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<NodeBase<Key, Value, Count, UniqueDiskPtr>, false>
	{
		using ThisType = NodeBase<Key, Value, Count, UniqueDiskPtr>;
		using MidNode = MiddleNode<Key, Value, Count, UniqueDiskPtr>;
		using LeafNode = LeafNode<Key, Value, Count, UniqueDiskPtr>;
		static constexpr bool SizeStable = false;

		static void WriteDown(ThisType const& node, shared_ptr<FileWriter> writer)
		{
			auto middle = node.Middle();
			ByteConverter<bool>::WriteDown(middle, writer);

			if (middle)
			{
				ByteConverter<MidNode>::WriteDown(static_cast<MidNode const&>(node), writer);
			}
			else
			{
				ByteConverter<LeafNode>::WriteDown(static_cast<LeafNode const&>(node), writer);
			}
		}

		static shared_ptr<ThisType> ReadOut(shared_ptr<FileReader> reader)
		{
			auto middle = ByteConverter<bool>::ReadOut(reader);

			if (middle)
			{
				return make_shared<MidNode>(ByteConverter<MidNode>::ReadOut(reader));
			}
			else
			{
				return make_shared<LeafNode>(ByteConverter<LeafNode>::ReadOut(reader));
			}
		}
	};
}
#pragma once
#include <type_traits>
#include <memory>
#include "../../Btree/Btree.hpp"
#include "../../Btree/NodeBase.hpp"
#include "../../Btree/MiddleNode.hpp"
#include "../../Btree/LeafNode.hpp"

namespace FuncLib::Persistence
{
	using Basic::Max;
	using ::Collections::Btree;
	using ::Collections::LeafNode;
	using ::Collections::MiddleNode;
	using ::Collections::NodeBase;
	using ::Collections::order_int;
	using ::Collections::StorePlace;
	using ::std::declval;
	using ::std::make_shared;
	using ::std::move;
	using ::std::shared_ptr;

	template <typename T>
	struct ByteConverter<UniqueDiskPtr<T>, false>
	{
		using ThisType = UniqueDiskPtr<T>;
		using DataMemberType = decltype(declval<ThisType>()._pos);
		static constexpr bool SizeStable = All<GetSizeStable, DataMemberType>::Result;
		static constexpr size_t Size = Sum<GetSize, DataMemberType>::Result;

		static void WriteDown(ThisType const& p, IWriter auto* writer)
		{
			ByteConverter<DataMemberType>::WriteDown(p._pos, writer, p._tPtr);
		}

		static ThisType ReadOut(FileReader* reader)
		{
			return ByteConverter<DataMemberType>::ReadOut(reader);
		}
	};

	template <typename T>
	struct ByteConverter<OwnerLessDiskPtr<T>, false>
	{
		using ThisType = OwnerLessDiskPtr<T>;
		using DataMemberType = decltype(declval<ThisType>()._pos);
		static constexpr bool SizeStable = All<GetSizeStable, DataMemberType>::Result;
		static constexpr size_t Size = Sum<GetSize, DataMemberType>::Result;

		static void WriteDown(ThisType const& p, IWriter auto* writer)
		{
			ByteConverter<DataMemberType>::WriteDown(p._pos, writer);
		}

		static ThisType ReadOut(FileReader* reader)
		{
			return ByteConverter<DataMemberType>::ReadOut(reader);
		}
	};

	template <typename Key, typename Value, order_int Order>
	struct ByteConverter<Btree<Order, Key, Value, StorePlace::Disk>, false>
	{
		using ThisType = Btree<Order, Key, Value, StorePlace::Disk>;
		using DataMemberType0 = decltype(declval<ThisType>()._keyCount);
		using DataMemberType1 = decltype(declval<ThisType>()._root);
		static constexpr bool SizeStable = All<GetSizeStable, DataMemberType0, DataMemberType1>::Result;
		static constexpr size_t Size = SizeStable ? Sum<GetSize, DataMemberType0, DataMemberType1>::Result : SIZE_MAX;

		static void WriteDown(ThisType const& t, IWriter auto* writer)
		{
			ByteConverter<DataMemberType0>::WriteDown(t._keyCount, writer);
			ByteConverter<DataMemberType1>::WriteDown(t._root, writer);
		}

		static ThisType ReadOut(FileReader* reader)
		{
			auto member0 = ByteConverter<DataMemberType0>::ReadOut(reader);
			auto member1 = ByteConverter<DataMemberType1>::ReadOut(reader);
			return { member0, move(member1) };
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<MiddleNode<Key, Value, Count, StorePlace::Disk>, false>
	{
		using ThisType = MiddleNode<Key, Value, Count, StorePlace::Disk>;
		using DataMemberType = decltype(declval<ThisType>()._elements);
		static constexpr bool SizeStable = All<GetSizeStable, DataMemberType>::Result;
		static constexpr size_t Size = SizeStable ? Sum<GetSize, DataMemberType>::Result : SIZE_MAX;

		static void WriteDown(ThisType const& t, IWriter auto* writer)
		{
			ByteConverter<DataMemberType>::WriteDown(t._elements, writer);
		}

		static ThisType ReadOut(FileReader* reader)
		{
			auto elements = ByteConverter<DataMemberType>::ReadOut(reader);
			return { move(elements) };
			// TODO provide LeafNode previous, next and callback inner 这里回调设置的事情还没有弄
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<LeafNode<Key, Value, Count, StorePlace::Disk>, false>
	{
		using ThisType = LeafNode<Key, Value, Count, StorePlace::Disk>;
		using DataMemberType0 = decltype(declval<ThisType>()._elements);
		using DataMemberType1 = decltype(declval<ThisType>()._previous);
		using DataMemberType2 = decltype(declval<ThisType>()._next);
		static constexpr bool SizeStable = All<GetSizeStable, DataMemberType0, DataMemberType1, DataMemberType2>::Result;
		static constexpr size_t Size = SizeStable ? Sum<GetSize, DataMemberType0, DataMemberType1, DataMemberType2>::Result : SIZE_MAX;

		static void WriteDown(ThisType const& t, IWriter auto* writer)
		{
			ByteConverter<DataMemberType0>::WriteDown(t._elements, writer);
			ByteConverter<DataMemberType1>::WriteDown(t._previous, writer);
			ByteConverter<DataMemberType2>::WriteDown(t._next, writer);
		}

		static ThisType ReadOut(FileReader* reader)
		{
			auto elements = ByteConverter<DataMemberType0>::ReadOut(reader);
			auto preivous = ByteConverter<DataMemberType1>::ReadOut(reader);
			auto next = ByteConverter<DataMemberType2>::ReadOut(reader);
			return { move(elements), preivous, next };
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<NodeBase<Key, Value, Count, StorePlace::Disk>, false>
	{
		using ThisType = NodeBase<Key, Value, Count, StorePlace::Disk>;
		using MidNode = MiddleNode<Key, Value, Count, StorePlace::Disk>;
		using LeafNode = LeafNode<Key, Value, Count, StorePlace::Disk>;
		static constexpr bool SizeStable = All<GetSizeStable, MidNode, LeafNode>::Result;
		static constexpr size_t Size = SizeStable ? Max<GetSize, MidNode, LeafNode>::Result : SIZE_MAX;

		static void WriteDown(ThisType const& node, IWriter auto* writer)
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

		static shared_ptr<ThisType> ReadOut(FileReader* reader)
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

	template <template <typename, typename, order_int, StorePlace> class Node, typename Key, typename Value, size_t Left, size_t Right>
	constexpr size_t FindSuitableNIn() // 不能用 size_t left, size_t right 直接作为参数
	{
		constexpr size_t mid = (Left + Right) / 2;
		// 下面这个有没有考虑当前的 s < DiskBlockSize ？
		if constexpr (mid == Left)
		{
			return Left;
		}
		else if constexpr (mid == Right)
		{
			return Right;
		}

		constexpr auto s = ByteConverter<Node<Key, Value, mid, StorePlace::Disk>>::Size;
		if constexpr (s < DiskBlockSize)
		{
			return FindSuitableNIn<Node, Key, Value, mid, Right>();
		}
		else
		{
			return FindSuitableNIn<Node, Key, Value, Left, mid>();
		}
	}

	/// Mid 和 Leaf 的数量可以不一样，不过不弄了
	template <template <typename, typename, order_int, StorePlace> class Node, typename Key, typename Value, order_int Count>
	constexpr size_t CloseToNodeMaxN()
	{
		static_assert(ByteConverter<Node<Key, Value, Count, StorePlace::Disk>>::SizeStable, "Node type must size stable");

		constexpr auto s = ByteConverter<Node<Key, Value, Count, StorePlace::Disk>>::Size;
		if constexpr (s < DiskBlockSize)
		{
			return CloseToNodeMaxN<Node, Key, Value, Count * 2>();
		}
		else if constexpr (s == DiskBlockSize)
		{
			return Count;
		}
		else
		{
			return FindSuitableNIn<Node, Key, Value, Count / 2, Count - 1>();
		}		
	}

	template <typename Key, typename Value>
	constexpr size_t ComputeNodeMaxN()
	{
		auto m1 = CloseToNodeMaxN<MiddleNode, Key, Value, 1>();
		auto m2 = CloseToNodeMaxN<LeafNode, Key, Value, 1>();
		return m1 < m2 ? m1 : m2;
	}

	// constexpr auto S = ComputeNodeMaxN<int, int>();
}
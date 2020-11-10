#pragma once
#include <type_traits>
#include <memory>
#include "../../Btree/Btree.hpp"
#include "../../Btree/NodeBase.hpp"
#include "../../Btree/MiddleNode.hpp"
#include "../../Btree/LeafNode.hpp"

namespace FuncLib::Persistence
{
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
			return { move(elements) };// TODO provide LeafNode previous, next and callback inner
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
		static constexpr bool SizeStable = false;

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
}
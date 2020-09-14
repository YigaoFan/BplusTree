#pragma once
#include <type_traits>
#include <memory>
#include "../Btree/Basic.hpp"
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

		static void ConvertToByte(ThisType const& p, shared_ptr<FileWriter> writer)
		{
			ByteConverter<decltype(p._pos)>::ConvertToByte(p._pos, writer);
		}

		static ThisType ConvertFromByte(shared_ptr<FileReader> reader)
		{
			return ByteConverter<decltype(declval<ThisType>()._pos)>::ConvertFromByte(file, startInFile);
		}
	};

	template <typename T>
	struct ByteConverter<SharedDiskPtr<T>, false>
	{
		using ThisType = SharedDiskPtr<T>;

		static void ConvertToByte(ThisType const& p, shared_ptr<FileWriter> writer)
		{
			ByteConverter<decltype(p._pos)>::ConvertToByte(p._pos, writer);
		}

		static ThisType ConvertFromByte(shared_ptr<FileReader> reader)
		{
			return ByteConverter<decltype(declval<ThisType>()._pos)>::ConvertFromByte(reader);
		}
	};

	template <typename Key, typename Value, order_int Order>
	struct ByteConverter<Btree<Order, Key, Value, UniqueDiskPtr>, false>
	{
		using ThisType = Btree<Order, Key, Value, UniqueDiskPtr>;

		static void ConvertToByte(ThisType const& t, shared_ptr<FileWriter> writer)
		{
#define CONVERT_UNIT(OBJ) ByteConverter<decltype(OBJ)>::ConvertToByte(OBJ, writer)
			CONVERT_UNIT(t._keyCount) + CONVERT_UNIT(t._root);
#undef CONVERT_UNIT
		}

		static ThisType ConvertFromByte(shared_ptr<FileReader> reader)
		{
#define CONVERT_UNIT(PROPERTY) ByteConverter<decltype(declval<ThisType>().PROPERTY)>::ConvertFromByte(reader)
			auto c = CONVERT_UNIT(_keyCount);
			auto r = CONVERT_UNIT(_root);
			return { c, move(r) };// TODO set callback inner
#undef CONVERT_UNIT
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<MiddleNode<Key, Value, Count, UniqueDiskPtr>, false>
	{
		using ThisType = MiddleNode<Key, Value, Count, UniqueDiskPtr>;

		static void ConvertToByte(ThisType const& t, shared_ptr<FileWriter> writer)
		{
			ByteConverter<decltype(t._elements)>::ConvertToByte(t._elements, writer);
		}

		static ThisType ConvertFromByte(shared_ptr<FileReader> reader)
		{
			using T = decltype(declval<ThisType>()._elements);
			auto elements = ByteConverter<T>::ConvertFromByte(reader);
			return { move(elements) };// TODO provide LeafNode previous, next and callback inner
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<LeafNode<Key, Value, Count, UniqueDiskPtr>, false>
	{
		using ThisType = LeafNode<Key, Value, Count, UniqueDiskPtr>;

		static void ConvertToByte(ThisType const& t, shared_ptr<FileWriter> writer)
		{
			ByteConverter<decltype(t._elements)>::ConvertToByte(t._elements, writer);
		}

		static ThisType ConvertFromByte(shared_ptr<FileReader> reader)
		{
			using T = decltype(declval<ThisType>()._elements);
			auto elements = ByteConverter<T>::ConvertFromByte(reader);
			return { move(elements) };
		}
	};

	template <typename Key, typename Value, order_int Count>
	struct ByteConverter<NodeBase<Key, Value, Count, UniqueDiskPtr>, false>
	{
		using ThisType = NodeBase<Key, Value, Count, UniqueDiskPtr>;
		using MidNode = MiddleNode<Key, Value, Count, UniqueDiskPtr>;
		using LeafNode = LeafNode<Key, Value, Count, UniqueDiskPtr>;

		static void ConvertToByte(ThisType const& node, shared_ptr<FileWriter> writer)
		{
			auto middle = node.Middle();
			ByteConverter<bool>::ConvertToByte(middle, writer);

			if (middle)
			{
				ByteConverter<MidNode>::ConvertToByte(static_cast<MidNode const&>(node), writer);
			}
			else
			{
				ByteConverter<LeafNode>::ConvertToByte(static_cast<LeafNode const&>(node), writer);
			}
		}

		static shared_ptr<ThisType> ConvertFromByte(shared_ptr<FileReader> reader)
		{
			auto middle = ByteConverter<bool>::ConvertFromByte(reader);

			if (middle)
			{
				return make_shared<MidNode>(ByteConverter<MidNode>::ConvertFromByte(reader));
			}
			else
			{
				return make_shared<LeafNode>(ByteConverter<LeafNode>::ConvertFromByte(reader));
			}
		}
	};
}
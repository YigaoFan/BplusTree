#pragma once
#include "Basic.hpp"
#include "TypeConfig.hpp"

namespace Collections
{
	template <typename Key, typename Value, order_int Order, StorePlace Place>
	class NodeBase;
	template <typename Key, typename Value, order_int Order, StorePlace Place>
	class MiddleNode;
	template <typename Key, typename Value, order_int Order, StorePlace Place>
	class LeafNode;

	template <typename T>
	struct GetNodeTypeFrom;

	template <typename Key, typename Value, order_int Order, StorePlace Place>
	struct GetNodeTypeFrom<NodeBase<Key, Value, Order, Place>>
	{
		using Leaf = LeafNode<Key, Value, Order, Place>;
		using Middle = LeafNode<Key, Value, Order, Place>;
	};

	template <typename Key, typename Value, order_int Order, StorePlace Place>
	struct GetNodeTypeFrom<LeafNode<Key, Value, Order, Place>>
	{
		using Base = NodeBase<Key, Value, Order, Place>;
	};

	template <typename Key, typename Value, order_int Order, StorePlace Place>
	struct GetNodeTypeFrom<MiddleNode<Key, Value, Order, Place>>
	{
		using Base = NodeBase<Key, Value, Order, Place>;
	};
}

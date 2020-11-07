#pragma once
#include "Basic.hpp"
#include "TypeConfig.hpp"

namespace Collections
{
	template <typename Key, typename Value, order_int BtreeOrder, StorePlace Place>
	class NodeBase;
	template <typename Key, typename Value, order_int BtreeOrder, StorePlace Place>
	class MiddleNode;
	template <typename Key, typename Value, order_int BtreeOrder, StorePlace Place>
	class LeafNode;

	template <typename T>
	struct GetNodeTypeFrom;

	template <typename Key, typename Value, order_int BtreeOrder, StorePlace Place>
	struct GetNodeTypeFrom<NodeBase<Key, Value, BtreeOrder, Place>>
	{
		using Leaf = LeafNode<Key, Value, BtreeOrder, Place>;
		using Middle = LeafNode<Key, Value, BtreeOrder, Place>;
	};
}

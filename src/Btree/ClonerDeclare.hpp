#pragma once
#include <type_traits>
#include "Basic.hpp"
#include "NodeBase.hpp"
#include "LeafNode.hpp"
#include "MiddleNode.hpp"

namespace Collections
{
	using ::std::result_of_t;

	template <typename Key, typename Value, order_int BtreeOrder, StorePlace Place>
	class MiddleNode;

	template <typename Key, typename Value, order_int Order, StorePlace Place>
	auto Clone(NodeBase<Key, Value, Order, Place> const* nodePtr) -> result_of_t<decltype(&MiddleNode<Key, Value, Order, Place>::Clone)(MiddleNode<Key, Value, Order, Place>)>;
}
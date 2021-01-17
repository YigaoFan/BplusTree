#pragma once
#include "Basic.hpp"
#include "NodeBase.hpp"
#include "LeafNode.hpp"
#include "MiddleNode.hpp"
#include "ClonerDeclare.hpp"

namespace Collections
{
	// virtual Ptr<NodeBase> Clone() const = 0;
	// 因为这个是虚方法，template class 必须编译，所以 disk 里 node 的也必须支持可复制，
	// 不想要这样，所以把它变成友元
	template <typename Key, typename Value, order_int Order, StorePlace Place>
	auto Clone(NodeBase<Key, Value, Order, Place> const* that) -> result_of_t<decltype(&MiddleNode<Key, Value, Order, Place>::Clone)(MiddleNode<Key, Value, Order, Place>)>
	{
		if (that->Middle())
		{
			using To = MiddleNode<Key, Value, Order, Place>;
			return static_cast<To const*>(that)->Clone();
		}
		else
		{
			using To = LeafNode<Key, Value, Order, Place>;
			return static_cast<To const*>(that)->Clone();
		}
	}
}
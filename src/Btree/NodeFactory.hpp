#pragma once
#include <memory>
#include <utility>
#include "Basic.hpp"
#include "LeafNode.hpp"
#include "MiddleNode.hpp"

namespace Collections
{
	using ::std::shared_ptr;
	using ::std::is_same_v;
	using ::std::pair;
	using ::std::unique_ptr;
	using ::std::make_unique;
	using ::std::decay_t;

	template <typename Key, typename Value, order_int BtreeOrder>
	class NodeFactory
	{
	private:
		using Node = NodeBase<Key, Value, BtreeOrder>;
		using LessThan = typename Node::LessThan;

	public:
		template <typename Iter>
		static unique_ptr<Node> MakeNode(Iter begin, Iter end, shared_ptr<LessThan> lessThan)
		{
			if constexpr (is_same_v<decay_t<decltype(*begin)>, pair<Key, Value>>)
			{
				return make_unique<LeafNode<Key, Value, BtreeOrder>>(begin, end, lessThan);
			}
			else
			{
				return make_unique<MiddleNode<Key, Value, BtreeOrder>>(begin, end, lessThan);
			}
		}
	};
}
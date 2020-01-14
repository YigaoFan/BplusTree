#pragma once
#include <memory>
#include "Basic.hpp"
#include "NodeBase.hpp"

namespace Collections
{
	using ::std::shared_ptr;

	template <typename Key, typename Value, order_int BtreeOrder>
	class NodeFactory
	{
	private:
		using Node = NodeBase<Key, Value, BtreeOrder>;
		using typename Node::LessThan;

	public:
		template <typename Iter>
		static Node MakeNode(Iter begin, Iter end, shared_ptr<LessThan> lessThan)
		{

		}

	private:
		template <typename Iter>
		static Node MakeLeafNode(Iter begin, Iter end, shared_ptr<LessThan> lessThan)
		{

		}

		template <typename Iter>
		static Node MakeMiddleNode(Iter begin, Iter end, shared_ptr<LessThan> lessThan)
		{

		}
	};
}
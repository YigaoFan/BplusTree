#pragma once
#include <utility>
#include <vector>
#include "Basic.hpp"
#include "NodeBase.hpp"
#include "LeafNode.hpp"
#include "MiddleNode.hpp"

namespace Collections
{
	using ::std::pair;
	using ::std::vector;

#define BASE   NodeBase  <Key, Value, BtreeOrder>
#define LEAF   LeafNode  <Key, Value, BtreeOrder>
#define MIDDLE MiddleNode<Key, Value, BtreeOrder>

	// for pre, should have nxt version
	template <bool IsLeaf, typename Key, typename Value, order_int BtreeOrder>
	void setNewPreRelation(BASE* newPreNode, BASE* currentNode)
	{
		// left is newPre, right is currentNode
		if constexpr (IsLeaf)
		{
			auto node = static_cast<LEAF*>(currentNode);
			auto oldPrevious = node->PreviousLeaf();
			auto newPre = static_cast<LEAF*>(newPreNode);

			newPre->PreviousLeaf(oldPrevious);
			newPre->NextLeaf(node);
			node->PreviousLeaf(newPre);
		}
	}

	template <bool IsLeaf, typename Key, typename Value, order_int BtreeOrder>
	void setRemoveCurrentRelation(BASE* currentNode)
	{
		if constexpr (IsLeaf)
		{
			auto current = static_cast<LEAF*>(currentNode);
			auto pre = current->PreviousLeaf();
			auto nxt = current->NextLeaf();

			pre->NextLeaf(nxt);
			nxt->PreviousLeaf(pre);
		}
	}

	template <bool IsLeaf, typename Key, typename Value, uint16_t BtreeOrder>
	void getPrevious(BASE* node, const vector<BASE*>& passedNodeTrackStack, BASE*& previous)
	{
		auto& stack = passedNodeTrackStack;
		if constexpr (IsLeaf)
		{
			auto leaf = static_cast<LEAF*>(node);
			previous = leaf->PreviousLeaf();
		}
		else
		{
			static_cast<MIDDLE*>(node)->searchPrevious(stack, previous);
		}
	}

	template <bool IsLeaf, typename Key, typename Value, order_int BtreeOrder>
	void getNext(BASE* node, const vector<BASE*>& passedNodeTrackStack, BASE*& next)
	{
		auto& stack = passedNodeTrackStack;
		if constexpr (IsLeaf)
		{
			auto leaf = static_cast<LEAF*>(node);
			next = leaf->NextLeaf();
		}
		else
		{
			static_cast<MIDDLE*>(node)->searchNext(stack, next);
		}
	}

#undef LEAF
#undef BASE
#undef MIDDLE
}

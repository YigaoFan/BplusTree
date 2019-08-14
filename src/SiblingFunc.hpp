#pragma once
#include "LeafNode.hpp"
#include "MiddleNode.hpp"

namespace btree {
	using std::pair;
	using std::vector;

#define BASE   NodeBase  <Key, Value, BtreeOrder>
#define LEAF   LeafNode  <Key, Value, BtreeOrder>
#define MIDDLE MiddleNode<Key, Value, BtreeOrder>

	// for pre, should have nxt version
	template <typename Key, typename Value, uint16_t BtreeOrder, bool IS_LEAF>
	void
	setNewPreRelation(BASE* newPreNode, BASE* currentNode)
	{
		// left is newPre, right is currentNode
		if constexpr (IS_LEAF) {
			auto node = static_cast<LEAF*>(currentNode);
			auto oldPrevious = node->previousLeaf();
			auto newPre = static_cast<LEAF*>(newPreNode);

			newPre->previousLeaf(oldPrevious);
			newPre->nextLeaf(node);
			node->previousLeaf(newPre);
		}
	}

	template <typename Key, typename Value, uint16_t BtreeOrder, bool IS_LEAF>
	void
	setRemoveCurrentRelation(BASE* currentNode)
	{
		if constexpr (IS_LEAF) {
			auto current = static_cast<LEAF*>(currentNode);
			auto pre = current->previousLeaf();
			auto nxt = current->nextLeaf();

			pre->nextLeaf(nxt);
			nxt->previousLeaf(pre);
		}
	}

	template <typename Key, typename Value, uint16_t BtreeOrder, bool IS_LEAF>
	void
	getPrevious(BASE* node, const vector<BASE*>& passedNodeTrackStack, BASE*& previous)
	{
		auto& stack = passedNodeTrackStack;

		if constexpr (IS_LEAF) {
			auto leaf = static_cast<LEAF*>(node);
			previous  = leaf->previousLeaf();
		} else {
			static_cast<MIDDLE*>(node)->searchPrevious(stack, previous);
		}
	}

	template <typename Key, typename Value, uint16_t BtreeOrder, bool IS_LEAF>
	void
	getNext(BASE* node, const vector<BASE*>& passedNodeTrackStack, BASE*& next)
	{
		auto& stack = passedNodeTrackStack;

		if constexpr (IS_LEAF) {
			auto leaf = static_cast<LEAF*>(node);
			next      = leaf->nextLeaf();
		} else {
			static_cast<MIDDLE*>(node)->searchNext(stack, next);
		}
	}

#undef LEAF
#undef BASE
}

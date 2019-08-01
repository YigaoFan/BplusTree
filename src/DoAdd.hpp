#pragma once
#include "LeafNode.hpp"
#include "MiddleNode.hpp"

namespace btree {
	using std::pair;
	using std::vector;

#define BASE   NodeBase  <Key, Value, BtreeOrder>
#define LEAF   LeafNode  <Key, Value, BtreeOrder>

	template <typename Key, typename Value, uint16_t BtreeOrder, typename T>
	void
	setSiblings(BASE* newPreNode, BASE* currentNode)
	{
		// left is newPre, right is currentNode
		if constexpr (std::is_same<typename std::decay<T>::type, Value>::value) {
			auto node = static_cast<LEAF*>(currentNode);
			auto oldPrevious = node->previousLeaf();
			auto preNode = static_cast<LEAF*>(newPreNode);

			preNode->previousLeaf(oldPrevious);
			preNode->nextLeaf(node);
			node->previousLeaf(preNode);
		}
	}

	template <typename Key, typename Value, uint16_t BtreeOrder, typename T>
	void
	getSiblings(BASE* node, vector<BASE*> passedNodeTrackStack, BASE*& previous, BASE*& next)
	{
		auto& stack = passedNodeTrackStack;

		if constexpr (std::is_same<typename std::decay<T>::type, Value>::value) {
			auto leaf = static_cast<LEAF*>(node);
			previous  = leaf->previousLeaf();
			next      = leaf->nextLeaf();
		} else {
			node->searchSiblingsIn(stack, previous, next);
		}
	}

#undef LEAF
#undef BASE
}

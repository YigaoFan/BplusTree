#pragma once
#include "LeafNode.hpp"
#include "MiddleNode.hpp"

namespace btree {
	using std::pair;
	using std::vector;

#define BASE   NodeBase  <Key, Value, BtreeOrder>
#define LEAF   LeafNode  <Key, Value, BtreeOrder>
#define MIDDLE MiddleNode<Key, Value, BtreeOrder>

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
	getPrevious(BASE* node, vector<BASE*> passedNodeTrackStack, BASE*& previous)
	{
		auto& stack = passedNodeTrackStack;

		if constexpr (std::is_same<typename std::decay<T>::type, Value>::value) {
			auto leaf = static_cast<LEAF*>(node);
			previous  = leaf->previousLeaf();
		} else {
			static_cast<MIDDLE*>(node)->searchPrevious(stack, previous);
		}
	}

	template <typename Key, typename Value, uint16_t BtreeOrder, typename T>
	void
	getNext(BASE* node, vector<BASE*> passedNodeTrackStack, BASE*& next)
	{
		auto& stack = passedNodeTrackStack;

		if constexpr (std::is_same<typename std::decay<T>::type, Value>::value) {
			auto leaf = static_cast<LEAF*>(node);
			next      = leaf->nextLeaf();
		} else {
			static_cast<MIDDLE*>(node)->searchNext(stack, next);
		}
	}

#undef LEAF
#undef BASE
}

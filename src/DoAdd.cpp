#include "NodeBase.hpp"
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
	getSiblings(BASE*, vector<BASE*>, BASE*&, BASE*&);
	template <typename Key, typename Value, uint16_t BtreeOrder>
	bool
	spaceFreeIn(const BASE*);

	template<typename Key, typename Value, uint16_t BtreeOrder, typename T>
	void
	doAdd(BASE* node, pair<Key, T> p, vector<BASE*>& passedNodeTrackStack)
	{
		// TODO need use std::move p or not?
		auto& k = p.first;
		auto& stack = passedNodeTrackStack;
		auto& lessThan = *(node->elements_.LessThanPtr);

		// some item don't have one of siblings
		BASE* previous = nullptr, next = nullptr;
		getSiblings<T>(node, stack, previous, next);

		if (!node->full()) {
			if (lessThan(k, node->maxKey())) {
				node->elements_.insert(p);
			} else {
				node->elements_.append(p);
				node->changeMaxKeyIn(stack, k);
			}
		} else if (spaceFreeIn(previous)) {
			node->reallocateSiblingElement(true,  stack, p);
		} else if (spaceFreeIn(next)) {
			node->reallocateSiblingElement(false, stack, p);
		} else {
			node->splitNode(p, stack);
		}
	}

	template <typename Key, typename Value, uint16_t BtreeOrder>
	void
	collectDeepInfo(BASE* startNode, const Key& key, vector<BASE*>& passedNodeTrackStack)
	{
		auto& stack = passedNodeTrackStack;

		if (!startNode->Middle) {
			stack.push_back(startNode);
		} else {
			static_cast<MIDDLE*>(startNode)->collectAddInfo(key, stack);
		}
	}

	template <typename Key, typename Value, uint16_t BtreeOrder, typename T>
	void
	getSiblings(BASE* node, vector<BASE*> passedNodeTrackStack, BASE*& previous, BASE*& next)
	{
		auto& stack = passedNodeTrackStack;

		if constexpr (std::is_same<typename std::decay<T>::type, Value>::value) {
			auto leaf = static_cast<LEAF*>(node);
			previous = leaf->previousLeaf();
			next     = leaf->nextLeaf();
		} else {
			node->searchSiblingsIn(stack, previous, next);
		}
	}

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

	template <typename Key, typename Value, uint16_t BtreeOrder>
	bool
	spaceFreeIn(const BASE* node)
	{
		if (node != nullptr) {
			return !node->full();
		}

		return false;
	}

#undef MIDDLE
#undef LEAF
#undef BASE
}

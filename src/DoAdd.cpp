#pragma once
#include "NodeBase.hpp"
#include "LeafNode.hpp"

namespace btree {
	using std::pair;
	using std::vector;

#define BASE NodeBase<Key, Value, BtreeOrder>
#define LEAF LeafNode<Key, Value, BtreeOrder>

	template<typename Key, typename Value, uint16_t BtreeOrder, typename T>
	void
	doAdd(BASE* node, pair<Key, T> p, vector<BASE*>& passedNodeTrackStack) {
		// TODO need use std::move p or not?
		auto &k = p.first;
		auto &lessThan = *(node->elements_.LessThanPtr);
		auto &stack = passedNodeTrackStack;

		// below should be a template function
		BASE* previous;
		BASE* next;
		if constexpr (std::is_same<typename std::decay<T>::type, Value>::value) {
			auto leaf = static_cast<LEAF*>(node);
			previous = leaf->previousLeaf();
			next     = leaf->nextLeaf();
		} else {
			node->searchSiblingsIn(stack, previous, next);
		}

		if (!node->full()) {
			if (lessThan(k, node->maxKey())) {
				node->elements_.insert(p);
			} else {
				node->elements_.append(p);
				node->changeMaxKeyIn(stack, k);
			}
		} else if (spaceFreeIn(previous)) {
			siblingElementReallocate(true, stack, p);
		} else if (spaceFreeIn(next)) {
			siblingElementReallocate(false, stack, p);
		} else {
			splitNode(p, stack);
		}
	}

#undef LEAF
#undef BASE
}

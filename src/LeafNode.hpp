#pragma once
#include <utility>
#include "NodeBaseCrtp.hpp"
//#include "Proxy.hpp"

namespace btree {
	template <typename Key, typename Value, uint16_t BtreeOrder>
	class MiddleNode;

#define NODE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>
#define LEAF LeafNode<Key, Value, BtreeOrder>

	NODE_TEMPLATE
	class LeafNode : public NodeBase_CRTP<LEAF, Key, Value, BtreeOrder> {
		using Base       = NodeBase<Key, Value, BtreeOrder>;
		using FatherType = MiddleNode<Key, Value, BtreeOrder>;
		using typename Base::LessThan;
	public:
		template <typename Iterator>
		LeafNode(Iterator, Iterator, shared_ptr<LessThan>);
		LeafNode(const LeafNode&, LeafNode* previous=nullptr,LeafNode* next=nullptr);
		LeafNode(LeafNode&&) noexcept;
		~LeafNode() override;

		const Value&     operator[](const Key&);
		pair<Key, Value> operator[](uint16_t);
		bool             add(pair<Key, Value>, vector<Base*>&);
		void             remove(const Key&);
		inline LeafNode* nextLeaf() const;
		inline void      nextLeaf(LeafNode*);
		// inline LeafNode* previousLeaf() const;
		inline void      previousLeaf(LeafNode*);

	private:
		// TODO remember to initialize two property below
		LeafNode* _next{ nullptr };
		LeafNode* _previous{ nullptr };

		inline bool spaceFreeIn(const LeafNode*) const;
		inline void siblingElementReallocate(bool, vector<Base *>&, pair<Key, Value>);
		inline void splitNode(pair<Key, Value>, vector<Base*>&);
		// template default is inline?
		inline void changeMaxKeyIn(vector<Base*>&, const Key&) const;
		inline void replacePreviousNodeMaxKeyInTreeBySearchUpIn(vector<Base*>&, const Key&, const Key&);
		inline void insertPreLeafToUpperNode(LeafNode*, vector<Base*>) const;

		Base* previousSearchIn(vector<Base*>&) const override;
		Base* nextSearchIn    (vector<Base*>&) const override;
	};
}

namespace btree {
	NODE_TEMPLATE
	template <typename Iter>
	LEAF::LeafNode(Iter begin, Iter end, shared_ptr<LessThan> funcPtr)
		: Base(LeafFlag(), begin, end, funcPtr)
	{}

	NODE_TEMPLATE
	LeafNode<Key, Value, BtreeOrder>::LeafNode(const LeafNode& that, LeafNode* previous, LeafNode* next)
		: Base(that), _previous(previous), _next(next)
	{}

	NODE_TEMPLATE
	LeafNode<Key, Value, BtreeOrder>::LeafNode(LeafNode&& that) noexcept
		: Base(std::move(that)), _next(that._next)
	{}

	NODE_TEMPLATE
	LeafNode<Key, Value, BtreeOrder >::~LeafNode() = default;

	NODE_TEMPLATE
	const Value&
	LEAF::operator[](const Key& key)
	{
		auto& e = this->elements_[key];
		return Base::Ele::value(e);
	}

	NODE_TEMPLATE
	pair<Key, Value>
	LEAF::operator[](uint16_t i)
	{
		auto& e = Base::elements_[i];
		return make_pair(e.first, Base::Ele::value(e.second));
	}

	NODE_TEMPLATE
	bool
	LEAF::add(pair<Key, Value> p, vector<Base*>& passedNodeTrackStack)
	{
#define MAX_KEY elements_[elements_.count() - 1].first

		using Base::full;
		using Base::elements_;
		auto& k = p.first;
		auto& lessThan = *(elements_.LessThanPtr);
		auto& stack = passedNodeTrackStack;
		stack.push_back(this);

		if (!full()) {
			if (lessThan(k, MAX_KEY)) {
				elements_.insert(p);
			} else {
				elements_.append(p);
				changeMaxKeyIn(stack, k);
			}
		} else if (spaceFreeIn(_previous)) {
			siblingElementReallocate(true, stack, p);
		} else if (spaceFreeIn(_next)) {
			siblingElementReallocate(false, stack, p);
		} else {
			splitNode(p, stack);
		}

#undef MAX_KEY
	}

	NODE_TEMPLATE
	void
	LEAF::remove(const Key& key)
	{
		if (this->elements_.remove(key)) {
//			btree_->change_bound_upwards(this,)
			// TODO
		}
	}

	NODE_TEMPLATE
	LEAF*
	LEAF::nextLeaf() const
	{
		return _next;
	}

	NODE_TEMPLATE
	void
	LEAF::nextLeaf(LeafNode* next)
	{
		_next = next;
	}

	NODE_TEMPLATE
	void
	LEAF::previousLeaf(LeafNode* previous)
	{
		_previous = previous;
	}

	NODE_TEMPLATE
	bool
	LEAF::spaceFreeIn(const LeafNode *node) const
	{
		return !node->full();
	}
	// TODO If you want to fine all the node, you could leave a gap "fineAllocate" to fine global allocate

	NODE_TEMPLATE
	void
	LEAF::siblingElementReallocate(bool isPrevious, vector<Base*>& passedNodeTrackStack, pair<Key, Value> p)
	{
		auto& stack = passedNodeTrackStack;
		// if max change, need to change above
		if (isPrevious) {
			auto&& min = Base::elements_.exchangeMin(p);
			auto&& previousOldMax = _previous->maxKey();
			_previous->elements_.append(min);
			// previous max change
			replacePreviousNodeMaxKeyInTreeBySearchUpIn(stack, _previous, min);
		} else {
			auto&& max = Base::elements_.exchangeMax(p);
			// this max change
			changeMaxKeyIn(stack, Base::maxKey());
			_next->elements_.insert(p);
		}
	}

	NODE_TEMPLATE
	void
	LEAF::splitNode(pair<Key, Value> p, vector<Base*>& passedNodeTrackStack)
	{
		auto& key = p.first;
		auto& lessThan = *(Base::Ele::LessThanPtr);
		auto& stack = passedNodeTrackStack;

		// construct a new one, left is newPre, right is this
		auto newPre = make_unique<LeafNode>(*this, _previous, this);
		auto newPrePtr = newPre.get();
		// update previous
		this->previousLeaf(newPrePtr);

		auto i = Base::elements_.suitablePosition(key);

		// [] not need reference if don't have last sentence?
		auto moveItems = [&] (uint16_t preNodeRemoveCount) {
			newPrePtr->elements_.removeItemsFrom(false, preNodeRemoveCount);
			Base::     elements_.removeItemsFrom(true,  BtreeOrder - preNodeRemoveCount);

			insertPreLeafToUpperNode(newPrePtr, stack); // stack is copied
		};

		auto addIn = [&] (LeafNode* leaf, bool shouldAppend) {
			if (shouldAppend) {
				leaf->elements_.append(p);
			} else {
				leaf->elements_.insert(p);
			}
		};
		auto handleAdd = [] (LeafNode* relatedLeaf, uint16_t maxBound) {
			auto isAppend = (i == maxBound);
			addIn(relatedLeaf, isAppend);
		};

		constexpr bool odd = BtreeOrder % 2;
		constexpr auto middle = odd ? (BtreeOrder / 2 + 1) : (BtreeOrder / 2);
		if (i <= middle) {
			constexpr auto removeCount = middle;
			moveItems(removeCount);

			handleAdd(newPrePtr, middle);
		} else {
			constexpr auto removeCount = BtreeOrder - middle;
			moveItems(removeCount);

			handleAdd(this, BtreeOrder);
		}
	}

	NODE_TEMPLATE
	void
	LEAF::changeMaxKeyIn(vector<Base*>& passedNodeTrackStack, const Key& maxKey) const
	{
		auto& stack = passedNodeTrackStack;
		if (stack.size() < 2) {
			return;
		}

		auto  nodePtr = stack.pop_back();
		auto& upperNode = *(stack[stack.size() - 1]);
		// 以下这个修改 key 的部分可以复用
		auto  matchIndex = upperNode.elements_.indexOf(nodePtr);
		upperNode[matchIndex].first = maxKey;

		auto maxIndex = upperNode->childCount() - 1;
		if (matchIndex == maxIndex) {
			changeMaxKeyIn(stack, maxKey);
		}
	}

	// use stack to get root node(maybe not need root node), then use this early max key to search
	NODE_TEMPLATE
	void
	LEAF::replacePreviousNodeMaxKeyInTreeBySearchUpIn(vector<Base*>& passedNodeTrackStack, const Key& oldKey, const Key& newKey)
	{
		auto& stack = passedNodeTrackStack;
#define EMIT_UPPER_NODE() stack.pop_back()

		EMIT_UPPER_NODE(); // top pointer is leaf, it's useless

		for (Base* node = EMIT_UPPER_NODE(); stack.size() != 0; node = EMIT_UPPER_NODE()) {
			// judge if the node is the same ancestor between this and previous node
			if (node->have(oldKey)) {
				node->changeInSearchDownPath(oldKey, newKey);

				// judge if need to change upper node
				auto i = node->elements_.indexOf(oldKey); // should create a method called this layer search
				auto maxIndex = Base::childCount() - 1;
				if (i != -1 && i == maxIndex) {
					stack.push_back(node);
					// change upper node
					changeMaxKeyIn(stack, newKey);
					break;
				}
			}
		}
	}

	NODE_TEMPLATE
	void
	LEAF::insertPreLeafToUpperNode(LeafNode* preLeaf, vector<Base*> passedNodeTrackStack) const
	{
		auto& stack = passedNodeTrackStack;
		EMIT_UPPER_NODE(); // top pointer is leaf, it's useless
		
		// 这部分甚至可能动 root，做好心理准备

		Base* node = EMIT_UPPER_NODE();

		if (!node->full()) {
			if (node->add(node)) {
				// change max key upper in stack
			}
		} else {
			// if sibling free, move to sibling

			// not free, split node into two, do the LeafNode same thing to upper node

			// attention to arrive root, and change root
		}
		// 这一层造好节点之后，就交给上一层处理
		
#undef EMIT_UPPER_NODE
	}

	NODE_TEMPLATE
	typename LEAF::Base*
	LEAF::previousSearchIn(vector<Base*>&) const
	{
		return _previous;
	}

	NODE_TEMPLATE
	typename LEAF::Base*
	LEAF::nextSearchIn(vector<Base*>&) const
	{
		return _next;
	}

#undef LEAF
#undef NODE_TEMPLATE
}
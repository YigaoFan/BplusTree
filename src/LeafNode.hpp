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
		LeafNode(const LeafNode&, LeafNode* next=nullptr);
		LeafNode(LeafNode&&) noexcept;
		~LeafNode() override;

		const Value&     operator[](const Key&);
		pair<Key, Value> operator[](uint16_t);
		bool             add(pair<Key, Value>, vector<Base*>&);
		void             remove(const Key&);
		inline LeafNode* nextLeaf() const;
		inline void      nextLeaf(LeafNode*);
		// FatherType*      father() const override;

	private:
		// TODO remember to initialize two property below
		LeafNode* _next{ nullptr };
		LeafNode* _previous{ nullptr };

		inline bool spaceFreeIn(const LeafNode*) const;
		inline void siblingElementReallocate(pair<Key, Value>);
		inline void splitNode(pair<Key, Value>);
		// template default is inline?
		template <bool FirstCall = true>
		inline void changeMaxKeyIn(vector<Base*>&, const Key&) const;
	};
}

namespace btree {
	NODE_TEMPLATE
	template <typename Iter>
	LEAF::LeafNode(Iter begin, Iter end, shared_ptr<LessThan> funcPtr)
		: Base(LeafFlag(), begin, end, funcPtr)
	{}

	NODE_TEMPLATE
	LeafNode<Key, Value, BtreeOrder>::LeafNode(const LeafNode& that, LeafNode* next)
		: Base(that), _next(next)
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
			// How to split some element to another
			siblingElementReallocate();
		} else if (spaceFreeIn(_next)) {
			// How to split some element to another
			siblingElementReallocate(p);
		} else {
			splitNode(p);
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

	// NODE_TEMPLATE
	// typename LEAF::FatherType*
	// LEAF::father() const
	// {
	// 	return static_cast<FatherType*>(Base::father());
	// }

	NODE_TEMPLATE
	bool
	LEAF::spaceFreeIn(const LeafNode *node) const
	{
		return !node->full();
	}
	// TODO If you want to fine all the node, you could leave a gap "fineAllocate" to fine global allocate

	NODE_TEMPLATE
	void
	LEAF::siblingElementReallocate(pair<Key, Value> p)
	{
		// if max change, need to change above
	}

	NODE_TEMPLATE
	void
	LEAF::splitNode(pair<Key, Value> p)
	{
		// if max change, need to change above
	}

	NODE_TEMPLATE
	template <bool FirstCall>
	void
	LEAF::changeMaxKeyIn(vector<Base*>& passedNodeTrackStack, const Key& maxKey) const
	{
		auto& stack = passedNodeTrackStack;
		auto node = *(passedNodeTrackStack.pop_back());

		if constexpr (FirstCall) {
			// TODO check below code
			auto nodePtr = stack.pop_back();
			auto i = Base::Ele::indexOf(nodePtr);
			auto upperNodePtr = stack[stack.size() - 1];
			auto matchIndex = upperNodePtr->elements_.indexOf(nodePtr);
			node[matchIndex].first = maxKey;
			auto maxIndex = upperNodePtr->childCount() - 1;

			if (matchIndex == maxIndex) {
				// Change upper recursively
			}
		} else {
			// Change corresponding key
			// Add a index() to Elements
			// If not max, stop to change

		}

	}
#undef LEAF
#undef NODE_TEMPLATE
}
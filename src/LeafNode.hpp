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
        bool             add(pair<Key, Value>);
        void             remove(const Key&);
        inline LeafNode* nextLeaf() const;
        inline void      nextLeaf(LeafNode*);
        FatherType*      father() const override;

	private:
		LeafNode* _next{ nullptr };
		LeafNode* _previous{ nullptr };

		unique_ptr<Base> clone() const override;
		inline bool previousSpaceFree() const;
		inline bool nextSpaceFree()     const;
		inline void siblingElementReallocate(pair<Key, Value>);
		inline void splitNode(pair<Key, Value>);
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
	LEAF::add(pair<Key, Value> p)
	{
		// TODO not very clear to the adjust first, or process other related Node first
#define MAX_KEY elements_[elements_.count() - 1].first

		using Base::elements_;
		using Base::full;
		auto& k = p.first;
		auto& lessThan = *(elements_.LessThanPtr);

		if (!full()) {
			if (lessThan(k, MAX_KEY)) {
				elements_.insert(p);
			} else {
				elements_.append(p);
				// Change bound in NodeBase above
			}
		} else if (previousSpaceFree()) {
			siblingElementReallocate(p);
		} else if (nextSpaceFree()) {
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

	NODE_TEMPLATE
	typename LEAF::FatherType*
	LEAF::father() const
	{
		return static_cast<FatherType*>(Base::father());
	}

    NODE_TEMPLATE
    unique_ptr<typename LEAF::Base>
    LEAF::clone() const
    {
        return make_unique<LEAF>(*this);
    }
    
    NODE_TEMPLATE
	bool
	LEAF::previousSpaceFree() const
	{
		// TODO
		// TODO If you want to fine all the node, you could leave a gap "fineAllocate" to fine global allocate
		return !_previous->full();
	}

	NODE_TEMPLATE
	bool
	LEAF::nextSpaceFree() const
	{
		// TODO
		// TODO If you want to fine all the node, you could leave a gap "fineAllocate" to fine global allocate
		return !_previous->full();
	}


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
#undef LEAF
#undef NODE_TEMPLATE
}
#pragma once
#include <utility>
#include "NodeBase.hpp"
#include "MiddleNode.hpp"
//#include "Proxy.hpp"

namespace btree {
#define LEAF_NODE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>

	LEAF_NODE_TEMPLATE
    class LeafNode final : public NodeBase<Key, Value, BtreeOrder> {
		using Base       = NodeBase<Key, Value, BtreeOrder>;
		using FatherType = MiddleNode<Key, Value, BtreeOrder>;
		using typename Base::CompareFunc;
	public:
        template <typename Iterator>
        LeafNode(Iterator, Iterator, shared_ptr<CompareFunc>);
		LeafNode(const LeafNode&, LeafNode* next=nullptr);
    	LeafNode(LeafNode&&) noexcept;
        ~LeafNode() override;

        const Value&     operator[](const Key&);
		pair<Key, Value> operator[](uint16_t);
        bool add(pair<Key, Value>);
        void             remove(const Key&);
        inline LeafNode* nextLeaf() const;
        inline void      nextLeaf(LeafNode&);
        FatherType*      father() const override;

	private:
		LeafNode* _next{ nullptr };
    };
}

// implement
namespace btree {
#define LEAF LeafNode<Key, Value, BtreeOrder>

	LEAF_NODE_TEMPLATE
	template <typename Iter>
	LEAF::LeafNode(Iter begin, Iter end, shared_ptr<CompareFunc> funcPtr)
		: Base(leafType(), begin, end, funcPtr)
    {}

	LEAF_NODE_TEMPLATE
	LeafNode<Key, Value, BtreeOrder>::LeafNode(const LeafNode& that, LeafNode* next)
		: Base(that), _next(next)
	{}

	LEAF_NODE_TEMPLATE
	LeafNode<Key, Value, BtreeOrder>::LeafNode(LeafNode&& that) noexcept
		: Base(std::move(that)), _next(that._next)
	{}

	LEAF_NODE_TEMPLATE
	LeafNode<Key, Value, BtreeOrder >::~LeafNode() = default;

    LEAF_NODE_TEMPLATE
    const Value&
	LEAF::operator[](const Key& key)
	{
		auto& e = this->elements_[key];
		return Base::Ele::value(e);
	}

	LEAF_NODE_TEMPLATE
	pair<Key, Value>
	LEAF::operator[](uint16_t i)
	{
		auto& e = this->elements_[i];
		return make_pair(e.first, Base::Ele::value(e.second));
	}

	LEAF_NODE_TEMPLATE
	bool
	LEAF::add(pair<Key, Value> p)
	{
//		if (this->elements_.full()) {
//			auto&& old_key = this->max_key();
//			pair<Key, Value> out_pair{};
//
//			if (this->elements_.exchange_max_out(p, out_pair)) {
//				this->btree_.change_bound_upwards(this, old_key, this->max_key());
//				// function name should be put pair in and exchange_max_out
//			}
//
//            // TODO not very clear to the adjust first, or process other related Node first
//
//            // next node add
//            if (this->next_node_ != nullptr) {
//                this->next_node_->element_add(out_pair);
//            } else {
//                this->father_add(out_pair);
//            }
//        } else {
//            auto&& old_key = this->max_key();
//
//            if (this->elements_.add(p)) { // return bool if maxKey changed
//                // call BtreeHelper
//                this->btree_.change_bound_upwards(this, old_key, this->max_key());
//            }
//        }
		if (this->elements_.CompareFuncPtr->operator()(p.first, this->maxKey())) {
			this->elements_.append(p);
			return true;
		} else {
			this->elements_.insert(p);
			return false;
		}
	}

	LEAF_NODE_TEMPLATE
	void
	LEAF::remove(const Key& key)
	{
		if (this->elements_.remove(key)) {
//			btree_->change_bound_upwards(this,)
			// TODO
		}
	}

	LEAF_NODE_TEMPLATE
	LEAF*
	LEAF::nextLeaf() const
	{
		return _next;
	}

	LEAF_NODE_TEMPLATE
	void
	LEAF::nextLeaf(LeafNode& next)
	{
		_next = &next;
	}

	LEAF_NODE_TEMPLATE
	typename LEAF::FatherType*
	LEAF::father() const
	{
		return static_cast<FatherType*>(Base::father());
	}

//	LEAF_NODE_TEMPLATE
//	Proxy<Key, Value, BtreeOrder, >
//	LeafNode<Key, Value, BtreeOrder, >::self()
//	{
//		return Proxy{this};
//	}

#undef LEAF
#undef LEAF_NODE_TEMPLATE
}
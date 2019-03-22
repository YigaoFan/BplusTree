#pragma once
#include <utility>
#include "NodeBase.hpp"

namespace btree {
    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    class LeafNode final : public NodeBase<Key, Value, BtreeOrder, BtreeType> {
	public:
        template <typename Iterator>
        LeafNode(BtreeType&, Iterator, Iterator);
        LeafNode* Clone(BtreeType&) const override;
        ~LeafNode() override;

        const Value& operator[](const Key&) const;
        void add(const std::pair<Key, Value>&);
        void remove(const Key&);
        inline LeafNode* next_leaf() const;
        inline void next_leaf(LeafNode*);

	private:
    	using Base = NodeBase<Key, Value, BtreeOrder, BtreeType>;
		LeafNode(const LeafNode&, BtreeType&, LeafNode* next=nullptr);

		LeafNode* next_{ nullptr };
//    	put here (private) to refuse call externally
    	LeafNode(LeafNode&&) noexcept;
    	LeafNode& operator=(const LeafNode&);
    	LeafNode& operator=(LeafNode&&) noexcept;
    };
}

// implementation
namespace btree {
	template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	template <typename Iter>
	LeafNode<Key, Value, BtreeOrder, BtreeType>::LeafNode(BtreeType& btree, Iter begin, Iter end)
		: Base(btree, leaf_type(), begin, end)
    {
		// null
    }

	template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	LeafNode<Key, Value, BtreeOrder, BtreeType>*
	LeafNode<Key, Value, BtreeOrder, BtreeType>::Clone(BtreeType& tree) const
	{
		auto l = new LeafNode(*this, tree);
		l->father(nullptr);
    	return l;
	}

	template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	LeafNode<Key, Value, BtreeOrder, BtreeType>::LeafNode(const LeafNode& that, BtreeType& tree, LeafNode* next)
		: Base(that, tree, leaf_type()), next_(next)
	{
		// copy constructor
	}

	template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	LeafNode<Key, Value, BtreeOrder, BtreeType>::~LeafNode() = default;

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    const Value&
	LeafNode<Key, Value, BtreeOrder, BtreeType>::operator[](const Key& key) const
	{
		auto& e = this->elements_[key];
		return this->Ele::ptr(e);
	}

	template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	void
	LeafNode<Key, Value, BtreeOrder, BtreeType>::add(const std::pair<Key, Value>& pair)
	{
		if (this->elements_.full()) {
			auto&& old_key = this->max_key();
			std::pair<Key, Value> out_pair{};

			if (this->elements_.exchange_max_out(pair, out_pair)) {
				this->btree_.change_bound_upwards(this, old_key, this->max_key());
				// function name should be put pair in and exchange_max_out
			}

            // TODO not very clear to the adjust first, or process other related Node first

            // next node add
            if (this->next_node_ != nullptr) {
                this->next_node_->element_add(out_pair);
            } else {
                this->father_add(out_pair);
            }
        } else {
            auto&& old_key = this->max_key();

            if (this->elements_.add(pair)) { // return bool if max_key changed
                // call BtreeHelper
                this->btree_.change_bound_upwards(this, old_key, this->max_key());
            }
        }
	}

	template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	void
	LeafNode<Key, Value, BtreeOrder, BtreeType>::remove(const Key& key)
	{
		auto save_key = this->max_key();
		if (this->elements_.remove(key)) {
//			btree_->change_bound_upwards(this,)
			// TODO
		}
	}

	template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	LeafNode<Key, Value, BtreeOrder, BtreeType>*
	LeafNode<Key, Value, BtreeOrder, BtreeType>::next_leaf() const
	{
		return next_;
	}

	template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	void
	LeafNode<Key, Value, BtreeOrder, BtreeType>::next_leaf(LeafNode* next)
	{
		next_ = next;
	}
}
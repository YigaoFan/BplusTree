#pragma once
#include <utility>
#include "NodeBase.hpp"

namespace btree {
    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    class LeafNode final : public NodeBase<Key, Value, BtreeOrder, BtreeType> {
        template <typename Iterator>
        LeafNode(Btree&, Iterator, Iterator);
        ~LeafNode() override;

        const Value& operator[](const Key&) const;
        void add(const std::pair<Key, Value>&);
        void remove(const Key&);
    };
}

// implementation
namespace btree {
	template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	template <typename Iter>
	LeafNode<Key, Value, BtreeOrder, BtreeType>::LeafNode(Btree& btree, Iter begin, Iter end)
		: NodeBase(btree, leaf_type(), begin, end)
    {
		// null
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	LeafNode<Key, Value, BtreeOrder, BtreeType>::~LeafNode()
    {

    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    const Value&
	LeafNode<Key, Value, BtreeOrder, BtreeType>::operator[](const Key& key) const
	{
		auto& e = elements_[key];
		return Elements::ptr(e);
	}

	template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	void
	LeafNode<Key, Value, BtreeOrder, BtreeType>::add(const std::pair<Key, Value>& pair)
	{
		// TODO there are some thing to sort
	}

	template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	void
	LeafNode<Key, Value, BtreeOrder, BtreeType>::remove(const Key& key)
	{
		auto save_key = this->max_key();
		if (elements_.remove(key)) {
//			btree_->change_bound_upwards(this,)
		}
	}
}
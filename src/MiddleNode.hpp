#pragma once
#include "NodeBase.hpp"

namespace btree {
    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    class MiddleNode final : public NodeBase<Key, Value, BtreeOrder, BtreeType> {
    private:
        using Base = NodeBase<Key, Value, BtreeOrder, BtreeType>;

    public:
        template <typename Iter>
        MiddleNode(BtreeType&, Iter, Iter);
        ~MiddleNode() override;

        Base* min_son() const;
        Base* max_son() const;
        Base* operator[](const Key&) const;
        MiddleNode* father() const override;

    private:
        MiddleNode(const MiddleNode&);

        // put here (private) to refuse call externally
        MiddleNode(MiddleNode&&) noexcept = delete;
    	MiddleNode& operator=(const MiddleNode&) = delete;
    	MiddleNode& operator=(MiddleNode&&) noexcept = delete;
    };
}

// implementation
namespace btree {
    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    template <typename Iter>
    MiddleNode<Key, Value, BtreeOrder, BtreeType>::MiddleNode(BtreeType& btree, Iter begin, Iter end)
        : Base(btree, middle_type(), begin, end)
    {
        // null
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    MiddleNode<Key, Value, BtreeOrder, BtreeType>::MiddleNode(const MiddleNode& that)
    {
    	// maybe wait to delete
        // TODO
    	// copy constructor
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    MiddleNode<Key, Value, BtreeOrder, BtreeType>::~MiddleNode()
    {
        // TODO wait complete
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    typename MiddleNode<Key, Value, BtreeOrder, BtreeType>::Base*
    MiddleNode<Key, Value, BtreeOrder, BtreeType>::min_son() const
    {
        return this->elements_.ptr_of_min();
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    typename MiddleNode<Key, Value, BtreeOrder, BtreeType>::Base*
    MiddleNode<Key, Value, BtreeOrder, BtreeType>::max_son() const
    {
        return this->elements_.ptr_of_max();
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    typename MiddleNode<Key, Value, BtreeOrder, BtreeType>::Base*
    MiddleNode<Key, Value, BtreeOrder, BtreeType>::operator[](const Key& key) const
    {
    	auto& e = this->elements_[key];
    	return this->Ele::value(e);
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    MiddleNode<Key, Value, BtreeOrder, BtreeType>*
    MiddleNode<Key, Value, BtreeOrder, BtreeType>::father() const
    {
        return static_cast<MiddleNode*>(Base::father());
    }
}
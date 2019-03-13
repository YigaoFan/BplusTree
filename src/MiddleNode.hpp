#pragma once
#include "NodeBase.hpp"

namespace btree {
    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    class MiddleNode final : public NodeBase<Key, Value, BtreeOrder, BtreeType> {
    public:
        template <typename Iter>
        MiddleNode(BtreeType&, Iter, Iter);
        ~MiddleNode() override;

        NodeBase* min_son() const;
        NodeBase* max_son() const;
    };
}

// implementation
namespace btree {
    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    template <typename Iter>
    MiddleNode<Key, Value, BtreeOrder, BtreeType>::MiddleNode(BtreeType& btree, Iter begin, Iter end)
        : NodeBase(btree, middle_type(), begin, end)
    {
        // null
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    MiddleNode<Key, Value, BtreeOrder, BtreeType>::~MiddleNode()
    {
        // TODO wait complete
    }

     template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
     NodeBase<Key, Value, BtreeOrder, BtreeType>*
     MiddleNode<Key, Value, BtreeOrder, BtreeType>::min_son() const
    {
         return elements_.ptr_of_min();
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    NodeBase<Key, Value, BtreeOrder, BtreeType>*
    MiddleNode<Key, Value, BtreeOrder, BtreeType>::max_son() const
    {
        return elements_.ptr_of_max();
    }
}
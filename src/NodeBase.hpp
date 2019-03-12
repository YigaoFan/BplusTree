#pragma once

#include <vector>
namespace btree {
    // maybe template arg can be less, like BtreeType maybe reduce
    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    class NodeBase {
    public:
        const bool middle;

        NodeBase(); // constructor
        virtual ~NodeBase();

        NodeBase* next_node() const;
        void next_node(NodeBase*);
        NodeBase* min_son() const;
        NodeBase* max_son() const;

    protected:
        NodeBase* next_{ nullptr };
        // Elements data
    };

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    NodeBase<Key, Value, BtreeOrder, BtreeType>*
        NodeBase<Key, Value, BtreeOrder, BtreeType>::next_node() const
    {
        return next_;
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    void
    NodeBase<Key, Value, BtreeOrder, BtreeType>::next_node(NodeBase* ptr)
    {
        next_ = ptr;
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    NodeBase<Key, Value, BtreeOrder, BtreeType>* NodeBase<Key, Value, BtreeOrder, BtreeType>::min_son() const
    {

    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    NodeBase<Key, Value, BtreeOrder, BtreeType>* NodeBase<Key, Value, BtreeOrder, BtreeType>::max_son() const
    {

    }
}

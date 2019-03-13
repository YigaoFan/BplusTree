#pragma once

#include "Elements.hpp" // for Elements

namespace btree {
    struct leaf_type {};
    struct middle_type {};

    // maybe template arg can be less, like BtreeType maybe reduce
    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    class NodeBase {
    public:
        const bool middle;
        BtreeType& btree;

        template <typename Iter>
        NodeBase(BtreeType&, leaf_type, Iter, Iter);
        template <typename Iter>
        NodeBase(BtreeType&, middle_type,Iter, Iter);
        virtual ~NodeBase();

        Key max_key() const;
        NodeBase* next_node() const;
        void next_node(NodeBase*);
        bool have(const Key&) const;
        void father(NodeBase*);

    protected:
        NodeBase* next_{ nullptr };
        NodeBase* father_{ nullptr };
        Elements<Key, Value, BtreeOrder, NodeBase> elements_;
    };


}

// implementation
namespace btree {
    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    template <typename Iter>
    NodeBase<Key, Value, BtreeOrder, BtreeType>::NodeBase(BtreeType& btree, leaf_type, Iter begin, Iter end)
    : middle(false), btree(btree), elements_(begin, end)
    {
        // null
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    template <typename Iter>
    NodeBase<Key, Value, BtreeOrder, BtreeType>::NodeBase(BtreeType& btree, middle_type, Iter begin, Iter end)
        : middle(true), btree(btree), elements_(begin, end)
    {
        // null
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    NodeBase<Key, Value, BtreeOrder, BtreeType>::~NodeBase()
    {
        // TODO wait complete
    }

     template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
     Key
     NodeBase<Key, Value, BtreeOrder, BtreeType>::max_key() const
    {
     	elements_.max_key();
    }

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
    bool
    NodeBase<Key, Value, BtreeOrder,BtreeType>::have(const Key& key) const
    {
    	if (middle) {
    	    return false;
    	} else {
    	    return elements_.have(key);
    	}
    }

    template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    NodeBase<Key, Value, BtreeOrder, BtreeType>*
    NodeBase<Key, Value, BtreeOrder, BtreeType>::father(NodeBase* father)
    {
        father_ = father;
    }
}

#pragma once

#include <vector> // for vector
#include "Elements.hpp" // for Elements
#include "Proxy.hpp"

namespace btree {
    struct leaf_type {};
    struct middle_type {};

    // maybe template arg can be less, like BtreeType maybe reduce
    template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
    class NodeBase {
    public:
        const bool Middle;
        BtreeType& btree; // TODO should replaced by compare_function&, maybe btree has not one function

        template <typename Iter>
        NodeBase(BtreeType&, leaf_type, Iter, Iter);
        template <typename Iter>
        NodeBase(BtreeType&, middle_type,Iter, Iter);
        virtual ~NodeBase();
        virtual Proxy<Key, Value, BtreeOrder, BtreeType> self() = 0;

        inline Key max_key() const;
        inline bool have(const Key&) const;
        inline std::vector<Key> all_key() const;
        virtual NodeBase* father() const = 0;
        int child_count() const;
    protected:
        using Ele = Elements<Key, Value, BtreeOrder, NodeBase>;
        NodeBase(const NodeBase&, BtreeType&, leaf_type);
        NodeBase(const NodeBase&) = delete; // maybe will add other copy control TODO

//    private:
        NodeBase* father_{ nullptr };
        Elements<Key, Value, BtreeOrder, NodeBase> elements_;

    };
}

// implementation
namespace btree {
    template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
    template <typename Iter>
    NodeBase<Key, Value, BtreeOrder, BtreeType>::NodeBase(BtreeType& btree, leaf_type, Iter begin, Iter end)
        : Middle(false), btree(btree), elements_(begin, end, btree.compareFunc)
    {
        // null
    }

    template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
    template <typename Iter>
    NodeBase<Key, Value, BtreeOrder, BtreeType>::NodeBase(BtreeType& btree, middle_type, Iter begin, Iter end)
        : Middle(true), btree(btree), elements_(begin, end, btree.compareFunc)
    {
        for (; begin != end; ++begin) {
            // need to use SFINE to control the property below?
            (*begin)->father_ = this;
        }
    }

    template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
    NodeBase<Key, Value, BtreeOrder, BtreeType>::NodeBase(const NodeBase& that, BtreeType& tree, leaf_type)
        : Middle(that.Middle), btree(tree), elements_(that.elements_, tree.compare_func)
    {
        // null
    }

    template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
    NodeBase<Key, Value, BtreeOrder, BtreeType>*
    NodeBase<Key, Value, BtreeOrder, BtreeType>::father() const
    {
    	return father_;
    }

    template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
    int
    NodeBase<Key, Value, BtreeOrder, BtreeType>::child_count() const
    {
		elements_.childCount();
    }

    template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
    NodeBase<Key, Value, BtreeOrder, BtreeType>::~NodeBase() = default;

    template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
    Proxy<Key, Value, BtreeOrder, BtreeType>
    NodeBase<Key, Value, BtreeOrder, BtreeType>::self()
    {
       return this;
    }

    template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
    Key
    NodeBase<Key, Value, BtreeOrder, BtreeType>::max_key() const
    {
        return elements_.maxKey();
    }

    template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
    bool
    NodeBase<Key, Value, BtreeOrder,BtreeType>::have(const Key& key) const
    {
        return elements_.have(key);
    }

    template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
    std::vector<Key>
    NodeBase<Key, Value, BtreeOrder, BtreeType>::all_key() const
    {
        return elements_.allKey();
    }
}

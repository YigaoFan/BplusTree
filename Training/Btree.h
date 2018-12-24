#pragma once
#ifndef BTREE_H
#define BTREE_H
#include <iostream> // for cin, cout
#include <vector> // for vector
#include <functional> // for lambda
#include <initializer_list> // for initializer_list
#include <memory> // for shared_ptr
#include <utility> // for pair
#include "Node.h"

// Should I think to setup a namespace
namespace btree {

    template<typename Key,
            typename Value,
            unsigned BtreeOrder,
            std::function<bool(Key, Key)> compare>
    class Btree {
    private:
        typedef Node<Key, Value, BtreeOrder> node_type;
        typedef std::function<bool(std::shared_ptr<const node_type>)> Predicate;
        std::shared_ptr<node_type> root_ = std::make_shared<node_type>(leaf);
        void adjust();
        node_type checkOut(Key);
        // provide some all-leaf-do operation?
        std::vector<Value>& traverseLeaf(Predicate);
        std::shared_ptr<NodeType> getSmallestLeafBack();
        std::shared_ptr<node_type> getFitLeafBack(typename node_type::PredicateFunc);

    public:
        Btree(std::initializer_list<std::pair<Key, Value>>);
        ~Btree();
        Value search(Key);
        int add(std::pair<Key, Value>);
        int modify(std::pair<Key, Value>);
        const std::vector<Key> explore();
        int remove(Key);
    };
}
#endif

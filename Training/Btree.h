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

namespace btree {
    // todo: ban using <
    template<typename Key, typename Value, unsigned BtreeOrder>
    class Btree {
    private:
        // type
        typedef Node<Key, Value, BtreeOrder> node_instance_type;
        typedef std::function<bool(std::shared_ptr<const node_instance_type>)> Predicate;
        
        // field
        std::shared_ptr<node_instance_type> root_ = std::make_shared<node_instance_type>(leaf);
        std::function<bool(Key, Key)> compare_function_;
        
        // method
        void adjust();
        std::shared_ptr<node_instance_type> check_out(Key);
        std::shared_ptr<node_instance_type> check_out_recur_helper(Key, node_instance_type);
        // provide some all-leaf-do operation?
        std::vector<Value>& traverseLeaf(Predicate);
        std::shared_ptr<NodeType> getSmallestLeafBack();
        std::shared_ptr<node_instance_type> getFitLeafBack(typename node_instance_type::PredicateFunc);

    public:
        Btree(std::initializer_list<std::pair<Key, Value>>, std::function<bool(Key, Key)> compare);
        ~Btree();
        Value search(Key);
        int add(std::pair<Key, Value>);
        int modify(std::pair<Key, Value>);
        const std::vector<Key> explore();
        int remove(Key);
    };
}
#endif

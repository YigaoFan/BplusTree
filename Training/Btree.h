#pragma once
#ifndef BTREE_H
#define BTREE_H
#include <vector> // for vector
#include <functional> // for lambda
#include <initializer_list> // for initializer_list
#include <memory> // for shared_ptr
#include <utility> // for pair
#include "Node.h"

namespace btree {
    // todo: zhihu bairubing 's answer can make compare function a template args
    template<typename Key, typename Value, unsigned BtreeOrder>
    class Btree {
    private:
        // type
        using node_instance_type = Node<Key, Value, BtreeOrder>;
        using compare_function_type = std::function<bool(Key const, Key const)>;
        using predicate = std::function<bool(std::shared_ptr<node_instance_type>)>;

    public:
        Btree(const std::initializer_list<std::pair<Key, Value>>, compare_function_type compare);
        ~Btree();
        Value search(const Key);
        int add(const std::pair<Key, Value>);
        int modify(const std::pair<Key, Value>);
        std::vector<Key> explore();
        void remove(const Key);

    private:
        // field
        std::shared_ptr<node_instance_type> root_ = std::make_shared<node_instance_type>(node_instance_type::leaf);
        compare_function_type compare_function_;
        
        // method
        //void adjust();
        std::shared_ptr<node_instance_type> check_out(const Key);
        std::shared_ptr<node_instance_type> check_out_recur_helper(const Key, std::shared_ptr<node_instance_type>);
        // provide some all-leaf-do operation
        std::vector<node_instance_type> traverse_leaf(predicate);
        std::shared_ptr<node_instance_type> get_smallest_leaf_back();
        //std::shared_ptr<node_instance_type> getFitLeafBack(node_instance_type::PredicateFunc);
    };
}
#endif

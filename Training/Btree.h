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
    // todo: should provide the default Compare type. Learn the less<arg>.
    template<typename Key, typename Value, typename Compare, unsigned BtreeOrder>
    class Btree {
    private:
        // friend can access the compare function
        // todo: wrong, Node how to access this member?
        friend Node<Key, Value, BtreeOrder>;
        // type
        using node_instance_type = Node<Key, Value, BtreeOrder>;
        // should I open the compare function type?  using compare_function_type = std::function<bool(const Key&, const Key&)>;
        using predicate = std::function<bool(std::shared_ptr<node_instance_type>)>;

    public:
        Btree(Compare&, const std::initializer_list<std::pair<Key, Value>>&);
        ~Btree();
        Value search(const Key&);
        RESULT_FLAG add(const std::pair<Key, Value>&);
        RESULT_FLAG modify(const std::pair<Key, Value>&);
        std::vector<Key> explore();
        void remove(const Key&);

    private:
        // field
        std::shared_ptr<node_instance_type> root_ = std::make_shared<node_instance_type>(node_instance_type::leaf);
        Compare compare_function_;
        
        // method
        //void adjust();
        std::shared_ptr<node_instance_type> check_out(const Key&);
        std::shared_ptr<node_instance_type> check_out_recur_helper(const Key&, const std::shared_ptr<node_instance_type>&);
        // provide some all-leaf-do operation
        std::vector<node_instance_type> traverse_leaf(const predicate&);
        std::shared_ptr<node_instance_type> get_smallest_leaf_back();
        //std::shared_ptr<node_instance_type> getFitLeafBack(node_instance_type::PredicateFunc);
    };
}
#endif

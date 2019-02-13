#pragma once

#include <vector> // for vector
#include <functional> // for lambda
#include <memory> // for shared_ptr
#include <utility> // for pair
#include "CommonFlag.h"
#include "Node.h"
#include "BtreeHelper.h"

namespace btree {
    template<typename Key, 
             typename Value,
             unsigned BtreeOrder>
    class Btree : private BtreeHelper{
    private:
        using node_instance_type = Node<Key, Value, BtreeOrder, Btree>;
        friend node_instance_type; // for Node use compare func, leaf_block_
        using predicate = std::function<bool(node_instance_type*)>;

    public:
        using compare = std::function<bool(const Key&, const Key&)>;
        // will change the vector arg
        template <unsigned long NumOfArrayEle> Btree(const compare&, std::array<std::pair<Key, Value>, NumOfArrayEle>&);
        template <unsigned long NumOfArrayEle> Btree(const compare&, std::array<std::pair<Key, Value>, NumOfArrayEle>&&);
        ~Btree() = default;
        Value search(const Key&) const;
        RESULT_FLAG add(const std::pair<Key, Value>&);
        RESULT_FLAG modify(const std::pair<Key, Value>&);
        std::vector<Key> explore() const;
        void remove(const Key&);
        bool have(const Key&);

    private:
        // Field
        std::unique_ptr<node_instance_type> root_{nullptr};
        const compare compare_func_;

        // key info
        unsigned key_num_; // TODO remember to add to Btree function

        //bool all_leaf_full() const;
        node_instance_type* check_out(const Key&);
        static node_instance_type* check_out_digging(const Key&, node_instance_type*);
        // provide some all-leaf-do operation
        std::vector<node_instance_type*> traverse_leaf(const predicate&);
        node_instance_type* smallest_leaf();
        node_instance_type* biggest_leaf();

        template <bool FirstFlag, typename Element, unsigned NodeCount> 
        void helper(const std::array<Element, NodeCount>&);

        //void root_add(const node_instance_type*, const std::pair<Key, Value>&);
        //void create_new_branch(const node_instance_type*, const std::pair<Key, Value>&);
        //void create_new_root(const node_instance_type*, const std::pair<Key, Value>&);
        void merge_branch(const Key, const node_instance_type*); // for Node merge a branch
    };
}

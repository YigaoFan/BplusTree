#pragma once

#include <vector> // for vector
#include <functional> // for lambda
#include <memory> // for shared_ptr
#include <utility> // for pair
#include "CommonFlag.h"
#include "Node.h"

namespace btree {
    template<typename Key, 
             typename Value,
             unsigned BtreeOrder>
    class Btree {
    private:
        using node_instance_type = Node<Key, Value, BtreeOrder, Btree>;
        friend node_instance_type; // for Node use compare func, leaf_block_
        using predicate = std::function<bool(node_instance_type*)>;

    public:
        using compare = std::function<bool(const Key&, const Key&)>;
        // will change the vector arg
        template <unsigned NumOfArrayEle> Btree(const compare&, std::array<std::pair<Key, Value>, NumOfArrayEle>&);
        template <unsigned NumOfArrayEle> Btree(const compare&, std::array<std::pair<Key, Value>, NumOfArrayEle>&&);
        ~Btree();
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
        /*Key min_key_;
        Key max_key_;*/

        bool all_leaf_full() const; // TODO maybe no using
        node_instance_type* check_out(const Key&);
        node_instance_type* check_out_recur(const Key&, 
            const node_instance_type*);
        // provide some all-leaf-do operation
        std::vector<node_instance_type*> traverse_leaf(const predicate&);
        node_instance_type* smallest_leaf();
        template <bool FirstFlag, typename Element, unsigned NodeCount> void helper(const std::array<Element, NodeCount>&);
        //template <typename T> static void set_father(typename T::iterator, const typename T::iterator&, void* father);
        //template <typename T> static void set_next_node(typename T::iterator, const typename T::iterator&);
    };
}

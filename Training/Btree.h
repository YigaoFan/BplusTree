#pragma once

#include <vector> // for vector
#include <functional> // for lambda
#include <memory> // for shared_ptr
#include <utility> // for pair
#include "CommonFlag.h"
#include "Node.h"

namespace btree {
    // should provide the default Compare type. Learn the less<arg>.
    template<typename Key, 
             typename Value,
             unsigned BtreeOrder,
             typename Compare = std::less<Key>>
    class Btree {
    private:
        using node_instance_type = Node<Key, Value, BtreeOrder, Btree>;
        using predicate = std::function<bool(std::shared_ptr<node_instance_type>)>;
        // Friend
        friend node_instance_type;

    public:
        // will change the vector arg
        template <unsigned NumOfArrayEle> Btree(const Compare, std::array<std::pair<Key, Value>, NumOfArrayEle>&);
        template <unsigned NumOfArrayEle> Btree(const Compare, std::array<std::pair<Key, Value>, NumOfArrayEle>&&);
        ~Btree();
        Value search(const Key&) const;
        RESULT_FLAG add(const std::pair<Key, Value>&);
        RESULT_FLAG modify(const std::pair<Key, Value>&);
        std::vector<Key> explore() const;
        void remove(const Key&);
        bool have(const Key&);

    private:
        // Field
        std::shared_ptr<node_instance_type> root_{nullptr};
        const Compare compare_func_;
        unsigned key_num_; // todo: remember to add to use

        std::weak_ptr<node_instance_type> check_out(const Key&);
        std::weak_ptr<node_instance_type> check_out_recur(const Key&, 
            const std::weak_ptr<node_instance_type>&);
        // provide some all-leaf-do operation
        std::vector<node_instance_type> traverse_leaf(const predicate&);
        std::shared_ptr<node_instance_type> smallest_leaf_back();
        template <bool FirstFlag, typename Element, unsigned NodeCount> void helper(std::array<Element, NodeCount>&);
        template <typename T> static void set_father(typename T::iterator, const typename T::iterator&, void* father);
    };
}

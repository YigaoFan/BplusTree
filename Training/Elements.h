#pragma once
#include <utility> // for pair
#include <array> // for array
#include <vector> // for vector
#include "Node.h"

namespace btree {
    template<typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    class Node<Key, Value, BtreeOrder, BtreeType>::Elements {
    public:
        Elements();
        Key max_key() const;
        bool have(const Key&) const;
        Node* min_value() const; // for Node for Btree traverse all leaf
        std::vector<Key> all_key() const;
        bool full() const;
        template <typename T>
        void add(const std::pair<Key, T>&);
        template <typename T>
        void add(const std::pair<Key, T*>&);
    private:
        std::array<std::pair<Key, std::variant<Value, std::unique_ptr<Node>>>, BtreeOrder> elements_{};
    };
};


    // TODO: move to Elements
//void move_Ele(const NodeIter<ele_instance_type>&, const NodeIter<ele_instance_type>&,
//              unsigned=1);



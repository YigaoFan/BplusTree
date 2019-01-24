#pragma once
#include <utility> // for pair
#include <array> // for array
#include <vector> // for vector
#include <variant> // for variant
#include <initializer_list>
#include "CommonFlag.h" // for project define flag 
#include "Node.h"

namespace btree {
    template<typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    class Node<Key, Value, BtreeOrder, BtreeType>::Elements {
    public:
        template <typename Iterator>
        Elements(const leaf_type, Iterator, Iterator);
        template <typename Iterator>
        Elements(const middle_type, Iterator, Iterator);
        /*template <typename T>
        Elements(const std::initializer_list<std::pair<Key, T*>>);*/
        // Common
        Key max_key() const;
        bool have(const Key&);
        std::vector<Key> all_key() const;
        bool full() const;

        // for Value
        Value& operator[](const Key&);
        template <typename T>
        void append(const std::pair<Key, T>&);
        // for ptr
        template <typename T>
        void append(const std::pair<Key, T*>&);
        Node* ptr_of_min() const; // for Node for Btree traverse all leaf

    private:
        // Field
        size_t count_;
        size_t cache_index_{0};
        std::array<std::pair<Key, std::variant<Value, std::unique_ptr<Node>>>, BtreeOrder> elements_;

        Value value(const std::variant<Value, std::unique_ptr<Node>>&) const;
        Node* ptr(const std::variant<Value, std::unique_ptr<Node>>&) const;
        void reset_cache();
    };
};


    // TODO: move to Elements
//void move_Ele(const NodeIter<ele_instance_type>&, const NodeIter<ele_instance_type>&,
//              unsigned=1);



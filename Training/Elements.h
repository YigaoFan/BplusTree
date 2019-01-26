#pragma once
#include <utility> // for pair
#include <array> // for array
#include <vector> // for vector
#include <variant> // for variant
#include "CommonFlag.h" // for project define flag 
#include "Node.h"

namespace btree {
    template<typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    class Node<Key, Value, BtreeOrder, BtreeType>::Elements {
    public:
        template <typename Iterator>
        Elements(Node*, Iterator, Iterator);
        /*template <typename T>
        Elements(const std::initializer_list<std::pair<Key, T*>>);*/
        // Common
        Key max_key() const;
        bool have(const Key&);
        std::vector<Key> all_key() const;
        void remove(const Key&);

        // for Value
        Value& operator[](const Key&);
        template <typename T>
        void append(const std::pair<Key, T>&);
        // for ptr
        //template <typename T>
        //void append(const std::pair<Key, T*>&);
        Node* ptr_of_min() const; // for Node for Btree traverse all leaf

    private:
        using content_type = std::pair<Key, std::variant<Value, std::unique_ptr<Node>>>;
        // Field
        Node* belong_node_;
        char count_;
        char cache_index_{0};
        Key cache_key_;
        std::array<content_type, BtreeOrder> elements_;

        void reset_cache();
        bool full() const;

        static Value value(const std::variant<Value, std::unique_ptr<Node>>&);
        static Node* ptr(const std::variant<Value, std::unique_ptr<Node>>&);
        content_type& move_element(content_type*, const char);
    };
};


    // TODO: move to Elements
//void move_Ele(const NodeIter<ele_instance_type>&, const NodeIter<ele_instance_type>&,
//              unsigned=1);



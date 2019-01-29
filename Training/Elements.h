#pragma once
#include <utility> // for pair
#include <array> // for array
#include <vector> // for vector
#include <variant> // for variant
#include <functional>
#include "CommonFlag.h" // for project define flag 
#include "Node.h"

namespace btree {
    template<typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
    class Node<Key, Value, BtreeOrder, BtreeType>::Elements {
    private:
        using content_type = std::pair<Key, std::variant<Value, std::unique_ptr<Node>>>;

    public:
        template <typename Iterator>
        Elements(Iterator, Iterator);
        /*template <typename T>
        Elements(const std::initializer_list<std::pair<Key, T*>>);*/
        // Common
        Key max_key() const;
        bool have(const Key&);
        std::vector<Key> all_key() const;
        bool full() const;
        void remove(const Key&);
        template <typename T> // use T to not limit to key-value
        bool add(const std::pair<Key, T>&);
        template <typename T>
        bool append(const std::pair<Key, T>&);


        // for Value
        Value& operator[](const Key&);
        
        std::pair<Key, Value> exchange_max_out(const std::pair<Key, Value>&); // when full call this function
        // for ptr
        Node* ptr_of_min() const; // for Node for Btree traverse all leaf
        Node* ptr_of_max() const; // for add the key beyond the max bound

    private:
        Elements() = default; // for construct null middle_type Node
        // Field
        char count_;
        char cache_index_{0};
        Key cache_key_;
        std::array<content_type, BtreeOrder> elements_;

        void reset_cache();

        static Value value(const std::variant<Value, std::unique_ptr<Node>>&);
        static Node* ptr(const std::variant<Value, std::unique_ptr<Node>>&);
        static content_type& move_element(const char, content_type*, content_type*);
        content_type& up_to_end_move(const char, content_type*);

    };
};


    // TODO: move to Elements
//void move_Ele(const NodeIter<ele_instance_type>&, const NodeIter<ele_instance_type>&,
//              unsigned=1);



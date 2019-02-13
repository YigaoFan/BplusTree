#pragma once 

#include <utility> // for pair
#include <vector> // for vector
#include <memory> // for unique_ptr
#include <variant>
#include "CommonFlag.h" // for project define flag
//#include "Elements.h"

namespace btree {
    struct leaf_type final {};
    struct middle_type final {};

    // When a Node is Created, its all type is done!
	template<typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	class Node {
		friend BtreeType; // for Btree set next_node_
		class Elements;
	public:
        const bool middle;

        template <typename Iterator>
        // point to key-value array
        Node(const BtreeType *, const leaf_type, Iterator, Iterator, const Node* = nullptr);
        template <typename Iterator>
        // point to key-ptr array
        Node(const BtreeType *, const middle_type, Iterator = nullptr, Iterator = nullptr, const Node* = nullptr);
        ~Node();

        bool have(const Key&);
		Value& operator[](const Key&);
        void add(const std::pair<Key, Value>&);
        void middle_append(const std::pair<Key, std::unique_ptr<Node>>&, const bool=false);
        void remove(const Key&);
        std::vector<Key> all_key() const;
        Key max_key() const;
        Node* min_leaf() const; // for Btree traverse get the leftest leaf
        Node* max_leaf() const; // for Btree get the rightest leaf
        void change_key(const Key&, const Key&);

	private:
		// Field
        Node* next_node_{nullptr};
        const BtreeType* btree_;
        Node* father_;
        Elements elements_;

        // Helper 
        bool full() const;
        void element_add(const std::pair<Key, Value>&);
        void father_add(const std::pair<Key, Value>&);

        class Elements {
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
            bool remove(const Key&);
            template <typename T> // use T to not limit to key-value
            bool add(const std::pair<Key, T>&);
            template <typename T>
            bool append(const std::pair<Key, T>&);

            // all here bool means change

            // for Value
            Value& operator[](const Key&);

            std::pair<Key, Value> exchange_max_out(const std::pair<Key, Value>&); // when full call this function
            // for ptr
            Node* ptr_of_min() const; // for Node for Btree traverse all leaf
            Node* ptr_of_max() const; // for add the key beyond the max bound
            bool change_key(const Key&, const Key&);

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
            content_type& to_end_move(const char, content_type*);
            content_type& related_position(const Key&);
        };
    };
}

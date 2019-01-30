#pragma once 

#include <utility> // for pair
#include <vector> // for vector
#include <memory> // for unique_ptr
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
        // Property
        const bool middle;

        // Construct
        template <typename Iterator>
        // point to key-value array
        Node(const BtreeType *, const leaf_type, Iterator, Iterator, const Node* = nullptr);
        template <typename Iterator>
        // point to key-ptr array
        Node(const BtreeType *, const middle_type, Iterator = nullptr, Iterator = nullptr, const Node* = nullptr);
        ~Node();

        // Method
        bool have(const Key&);
		Value& operator[](const Key&);
        void add(const std::pair<Key, Value>&);
        void middle_append(const std::pair<Key, std::unique_ptr<Node>>&, const bool=false);
        void remove(const Key&);
        std::vector<Key> all_key() const;
        Key max_key() const;
        Node* min_leaf() const; // for Btree traverse get the leftest leaf
        Node* max_leaf() const; // for Btree get the rightest leaf

	private:
		// Field
        Node next_node_{nullptr};
        const BtreeType* btree_;
        Node* father_;
        Elements elements_;

        // Helper 
        bool full() const;
        void element_add(const std::pair<Key, Value>&);
        void father_add(const std::pair<Key, Value>&);
	};
}

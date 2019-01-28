#pragma once 

#include <utility> // for pair
#include <vector> // for vector
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
        //Node(const BtreeType*, const Node*, const leaf_type, const std::pair<Key, Value>&); // for Btree add
        template <typename Iterator>
        // point to key-value array
        Node(const BtreeType *, const leaf_type, Iterator, Iterator, const Node* = nullptr);
        template <typename Iterator>
        // point to key-ptr array
        Node(const BtreeType *, const middle_type, Iterator, Iterator, const Node* = nullptr);
        ~Node();

        // Method
        bool have(const Key&);
		Value& operator[](const Key&);
        void add(const std::pair<Key, Value>&);
        void remove(const Key&);
        std::vector<Key> all_key() const;
        Key max_key() const;
        Node* min_leaf() const; // for Btree traverse get the leftest leaf
        Node* max_leaf() const; // for Btree traverse get the rightest leaf

	private:
		// Field
        Node next_node_{nullptr};
        const BtreeType* btree_;
        Node* father_;

        Elements elements_;

        // Helper 
        bool full() const;
            // for add
        /*RESULT_FLAG no_area_add(std::pair<Key, Value>);
        RESULT_FLAG area_add(const std::pair<Key, Value>&);*/

        void element_add(const std::pair<Key, Value>&);
        void middle_add(const std::pair<Key, Value>&);
            // for add and remove
        void adjust();

        // Old function
        //RESULT_FLAG insert(std::shared_ptr<ele_instance_type>);
//        void do_insert(std::shared_ptr<ele_instance_type>);
//        void exchange_the_biggest_ele_out(std::shared_ptr<ele_instance_type>);
        // make the method below like this createNewRoot(node1, node2...);
//        static RESULT_FLAG createNewRoot(const std::shared_ptr<Node>&, const std::shared_ptr<ele_instance_type> &);
//        static std::shared_ptr<ele_instance_type> constructToMakeItFitTheFatherInsert(std::shared_ptr<ele_instance_type>);
	};
}

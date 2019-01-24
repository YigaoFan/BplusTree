#pragma once 

#include <memory> // for shared_ptr
#include <utility> // for pair
#include <vector> // for vector
#include <variant> // for variant
#include <utility> // for pair
#include "CommonFlag.h" // for project define marco
#include "NodeIter.h" // for NodeIter
//#include "Elements.h"

namespace btree {
    struct leaf_type final {};
    struct middle_type final {};

    // When a Node is Created, its all type is done!
	template<typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	class Node {
		friend BtreeType; // for Btree set father, next_node_
		class Elements;
	private:
		// common construct
		Node(const BtreeType*, const Node*, const leaf_type);
		Node(const BtreeType*, const Node*, const middle_type);

	public:
        // Property
        const bool middle;

        // Construct
        Node(const BtreeType*, const Node*, const leaf_type, const std::pair<Key, Value>&); // for Btree add
        // TODO: remember to save the biggest Key, or other way can access it
        template <typename Container>
        Node(const BtreeType *, const leaf_type,
			 typename Container::iterator, // point to key-min_value array
			 typename Container::iterator,
            const Node* = nullptr);
        template <typename Container>
        Node(const BtreeType *, const middle_type,
			 typename Container::iterator, // point to ptr array
			 typename Container::iterator,
            const Node* = nullptr);
        ~Node();

		// Iterator
		// TODO: maybe should belong to Elements
		//NodeIter<ele_instance_type> begin();
		//NodeIter<ele_instance_type> end();

        // Method
        bool have(const Key&);
		Value& operator[](const Key&);
        RESULT_FLAG add(const std::pair<Key, Value>&);
        void remove(const Key&);
        std::vector<Key> all_key() 
	    const { return elements_.all_key(); }
        // return min_value means read-only
        Key max_key() const { return elements_.max_key(); }
        Node* min_value() const { return elements_.min_value(); } // for Btree traverse get the leftest leaf

	private:
		// Field
        Node next_node_{nullptr};
        const BtreeType* btree_;
        Node* father_;

        Elements elements_; // TODO can construct in Node constructor using indefinite para

        // Helper 
        bool full() { return elements_.full(); }
            // for add
        RESULT_FLAG no_area_add(std::pair<Key, Value>);
        RESULT_FLAG area_add(const std::pair<Key, Value>&);
        RESULT_FLAG middle_node_add(const std::pair<Key, Value>&);
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

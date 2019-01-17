#pragma once 

#include <memory> // for shared_ptr
#include <utility> // for pair
#include "CommonFlag.h" // for project define marco
#include "NodeIter.h" // for NodeIter
#include "Elements.h"
#include <vector>

namespace btree {
    struct leaf_type {};
    struct middle_type {};

    // When a Node is Created, its all type is done!
	template<typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	class Node {
		friend BtreeType; // for Btree set father
	public:
        // Property
        const bool middle;

        // Construct
        Node(const BtreeType*, const Node*, const leaf_type, const std::pair<Key, Value>&);
        Node(const BtreeType*, const Node*, const middle_type); // TODO: have using? now not implement
        // not change this iterator
        // TODO: remember to save the biggest Key, or other way can access it
        template <typename Container>
        Node(const BtreeType *, const leaf_type,
			 typename Container::iterator, // point to key-value array
				// below is tail of range, not included to construct Node
			 typename Container::iterator,
            const Node* = nullptr);
        template <typename Container>
        Node(const BtreeType *, const middle_type,
			 typename Container::iterator, // point to key-ptr array
				// below is tail of range, not included to construct Node
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
	    const { return elements_.all_key(key_num_); }

	private:
		// Field
		Elements<Key, BtreeOrder> elements_;
        decltype(elements_.size()) elements_count_{0};
        std::shared_ptr<Node> next_node_{nullptr};  // todo: complete
        const BtreeType* btree_;
        Node* father_;
        unsigned key_num_;

        // Helper 
        //bool has_child() { return middle && elements_count_ > 0; }
        bool full() { return elements_count_ >= BtreeOrder; }
            // for add
        RESULT_FLAG no_area_add(std::pair<Key, Value>);
        RESULT_FLAG area_add(const std::pair<Key, Value>&);
        RESULT_FLAG middle_node_add(const std::pair<Key, Value>&);
            // for add and remove

        void adjust();

        // Old function
        //RESULT_FLAG insert(std::shared_ptr<ele_instance_type>);
        void do_insert(std::shared_ptr<ele_instance_type>);
        void exchange_the_biggest_ele_out(std::shared_ptr<ele_instance_type>);
        // make the method below like this createNewRoot(node1, node2...);
        static RESULT_FLAG createNewRoot(const std::shared_ptr<Node>&, const std::shared_ptr<ele_instance_type> &);
        static std::shared_ptr<ele_instance_type> constructToMakeItFitTheFatherInsert(std::shared_ptr<ele_instance_type>);
	};
}

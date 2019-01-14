#pragma once // ensure included once

#include <array> // for array
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
	public:
        // Property
        const bool middle;

        // Construct
        Node(const BtreeType*, const Node*, const leaf_type);
        Node(const BtreeType*, const Node*, const middle_type);
        Node(const BtreeType*, const Node*,
            typename std::vector<std::pair<Key, Value>>::iterator&,
            // below is tail of range, not included to construct Node
            typename std::vector<std::pair<Key, Value>>::iterator&);
        // todo: maybe code the up construct method in Ele and PointerEle way
        ~Node();

		// Iterator
		// TODO: maybe should belong to Elements
		//NodeIter<ele_instance_type> begin();
		//NodeIter<ele_instance_type> end();

        // Method
		std::shared_ptr<ele_instance_type> operator[](const Key&&); // todo: can use &&
        RESULT_FLAG add(const std::pair<Key, Value>&);
        void remove(const Key&);

	private:
		// Field
		Elements<Key, BtreeOrder> elements_;
        decltype(elements_.size()) elements_count_{0};
        std::shared_ptr<Node> next_node_{nullptr};  // todo: complete
        const BtreeType* btree_;
        Node* father_;

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

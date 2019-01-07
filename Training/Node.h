#pragma once // ensure included once

#include <array> // for array
#include <memory> // for shared_ptr
#include <utility> // for pair
#include "CommonFlag.h" // for project define marco
#include "NodeIter.h" // for NodeIter

namespace btree {

    template <typename Key, typename Value, typename NodeType>
    struct Ele {
        // todo: the key type should provide a default value to make
        // todo: default construct complete
        union {
            // pair<child_value_low_bound, child>
            std::pair<Key, std::shared_ptr<NodeType>> intermediate_node;
            // pair<key, data>
            std::pair<Key, Value> leaf;
        };

        // todo: the shared_ptr will come together with low_bound?
        Ele(const std::pair<Key, std::shared_ptr<NodeType>>& pair)
        : intermediate_node(pair) {}

        Ele(const std::pair<Key, Value>& pair)
        : leaf(pair) {}

        // todo: need the function below?
        Ele(const Ele&);
        Ele &operator=(const Ele &e);
        // todo: the de-constructor maybe need to update
        ~Ele();

        // related to the data structure of Ele in Node
        Ele* next() { return this + 1; }
    };

	template<typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	class Node {
    private:
        using ele_instance_type = Ele<Key, Value, Node>;

	public:
        // Property
        const bool middle;

        // Construct
        explicit Node(const bool&, const BtreeType*);
        Node(const bool&, const BtreeType*, const std::pair<Key, Value>&);
        ~Node();

		// Iterator
		NodeIter<ele_instance_type> begin();
		NodeIter<ele_instance_type> end();

        // Method
		std::shared_ptr<ele_instance_type> operator[](Key);
        RESULT_FLAG add(const std::pair<Key, Value>&);
        void remove(const Key&);

	private:
		// Field
		std::array<std::shared_ptr<ele_instance_type>, BtreeOrder> elements_; // todo: construct
        decltype(elements_.size()) elements_count_{0};
        std::shared_ptr<Node> next_brother_{nullptr};
        std::shared_ptr<Node> father_{nullptr};
        const BtreeType* btree_;

        // Helper 
        bool has_child() { return middle && elements_count_ > 0; }
        bool is_full() { return elements_count_ >= BtreeOrder; }
            // for add
        RESULT_FLAG leaf_full_then_add(std::pair<Key, Value>);
        RESULT_FLAG leaf_has_area_add(const std::pair<Key, Value>&);
        RESULT_FLAG intermediate_node_add(const std::pair<Key, Value>&);
            // for add and remove
        void move_Ele(const NodeIter<ele_instance_type>&, const NodeIter<ele_instance_type>&,
                              unsigned);

        // Old function
        //RESULT_FLAG insert(std::shared_ptr<ele_instance_type>);
        void do_insert(std::shared_ptr<ele_instance_type>);
        void exchange_the_biggest_ele_out(std::shared_ptr<ele_instance_type>);
        // todo: make the method below like this createNewRoot(node1, node2...);
        static RESULT_FLAG createNewRoot(const std::shared_ptr<Node>&, const std::shared_ptr<ele_instance_type> &);
        static std::shared_ptr<ele_instance_type> constructToMakeItFitTheFatherInsert(std::shared_ptr<ele_instance_type>);
	};
}

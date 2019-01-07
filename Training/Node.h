#pragma once // ensure included once

#include <array> // for array
#include <memory> // for shared_ptr
#include <utility> // for pair
#include "CommonFlag.h" // for project define marco
#include "NodeIter.h" // for NodeIter

namespace btree {

    template <typename Key, typename Value, typename NodeType>
    struct Ele {
        const bool middle;
        // todo: the key type should provide a default value to make
        // todo: default construct complete
        union {
            // <child_value_low_bound, child>
            // todo: should save this, why make child type as Value type
            std::pair<Key, std::shared_ptr<NodeType>> intermediate_node;
            // <key, data>
            std::pair<Key, Value> leaf;
        };

        // todo: the shared_ptr will come together with low_bound?
        Ele(const std::pair<Key, std::shared_ptr<NodeType>>& p)
        : intermediate_node(p), middle(true) {}

        Ele(const std::pair<Key, Value>& p)
        : leaf(p), middle(false) {}

        // todo: need the function below?
        Ele(const Ele&);
        Ele &operator=(const Ele &e);
        // todo: the de-constructor maybe need to update
        ~Ele();

        // related to the data structure of Ele in Node
        Ele* next() { return this + 1; }

        Key& key() const { return middle ? intermediate_node.first : leaf.first; }
        Value& value() const { return middle ? intermediate_node.second : leaf.second; }
    };

    // When a Node is Created, its all type is done!
	template<typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
	class Node {
    private:
        using ele_instance_type = Ele<Key, Value, Node>;

	public:
        // Property
        const bool middle;

        // Construct
        explicit Node(const bool&, const BtreeType*, const Node*);
        Node(const bool&, const BtreeType*, const std::pair<Key, Value>&, const Node*);
        ~Node();

		// Iterator
		NodeIter<ele_instance_type> begin();
		NodeIter<ele_instance_type> end();

        // Method
		std::shared_ptr<ele_instance_type> operator[](const Key&&); // todo: can use &&
        RESULT_FLAG add(const std::pair<Key, Value>&);
        void remove(const Key&);

	private:
		// Field
		std::array<std::shared_ptr<ele_instance_type>, BtreeOrder> elements_; // todo: construct
        decltype(elements_.size()) elements_count_{0};
        std::shared_ptr<Node> next_node_{nullptr};  // todo: complete
        const BtreeType* btree_;
        Node* father_;

        // Helper 
         //bool has_child() { return middle && elements_count_ > 0; }
        bool full() { return elements_count_ >= BtreeOrder; }
            // for add
        RESULT_FLAG full_add(std::pair<Key, Value>);
        RESULT_FLAG area_add(const std::pair<Key, Value>&);
        RESULT_FLAG middle_node_add(const std::pair<Key, Value>&);
            // for add and remove
        void move_Ele(const NodeIter<ele_instance_type>&, const NodeIter<ele_instance_type>&,
                              unsigned=1);
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

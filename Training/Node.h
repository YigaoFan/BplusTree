#pragma once // ensure included once
#ifndef NODE_H
#define NODE_H

#include <vector> // for vector
#include <array> // for array
#include <memory> // for shared_ptr
#include <utility> // for pair
#include "CommonFlag.h" // for project define marco
#include "NodeIter.h" // for NodeIter

namespace btree {

    template <typename Key, typename Value, typename NodeType>
    struct Ele {
        // todo: the key type should provide a default value to make
        // todo: default construct complete.
        union {
            // intermediate_node
            struct {
                Key child_value_low_bound;
                std::shared_ptr<NodeType> child;
            };
            // leaf
            struct {
                Key key;
                Value data;
            };
        };

        Ele(const Key& low_bound, const std::shared_ptr<NodeType>& node) : child_value_low_bound(low_bound), child(node) {}
        Ele(const Key& key, const Value& data) : key(key), data(data) {}
        // todo: need the function below?
        Ele(const Ele&);
        Ele &operator=(const Ele &e);
        // todo: the de-constructor maybe need to update
        ~Ele();

        // related to the data structure of Ele in Node
        Ele* next() { return this + 1; }
    };

	template<typename Key, typename Value, unsigned BtreeOrder>
	class Node {
	private:
		using node_category = enum { intermediate_node = 1, leaf, };
		const node_category type_;

	public:
        using ele_instance_type = Ele<Key, Value, Node>;
		explicit Node(const node_category&);
		Node(const node_category&, const ele_instance_type&);
	    ~Node();
        RESULT_FLAG insert(std::shared_ptr<ele_instance_type>);
        std::shared_ptr<Node> next_brother{ nullptr };

	private:
		std::vector<std::shared_ptr<ele_instance_type>>& get_vector_of_elements_ref();
		// has_child will ensure the node isn't leaf and elements_count_ isn't 0
		bool has_child() { return type_ != leaf && elements_count_ > 0; }
		bool is_full() { return elements_count_ >= BtreeOrder; }
		void do_insert(std::shared_ptr<ele_instance_type>);
		void exchange_the_biggest_ele_out(std::shared_ptr<ele_instance_type>);

	private:
		std::shared_ptr<Node> father_{ nullptr };

		// tool function
		// todo: make the method below like this createNewRoot(node1, node2...);
		static RESULT_FLAG createNewRoot(const std::shared_ptr<Node>&, const std::shared_ptr<ele_instance_type> &);
		static std::shared_ptr<ele_instance_type> constructToMakeItFitTheFatherInsert(std::shared_ptr<ele_instance_type>);

		//new refactor:
	public:
		// for constructing iterator
		NodeIter<ele_instance_type> begin();
		NodeIter<ele_instance_type> end();

		std::shared_ptr<ele_instance_type> operator[](Key);

		// for check
		bool is_leaf() const { return type_ == leaf; }

		// add Ele or Node
        RESULT_FLAG add(const std::pair<Key, Value>&);

        void remove(const Key&);

        void shift_back_one_ele()

	private:
		// todo: use nodeType should test not produce error
		// todo: why here use shared_ptr<Ele>?
		std::array<std::shared_ptr<ele_instance_type>, BtreeOrder> elements_;
        decltype(elements_.size()) elements_count_{ 0 };

        // helper
        RESULT_FLAG leaf_full_then_add(const std::pair<Key, Value>&);
        RESULT_FLAG leaf_has_area_add(const std::pair<Key, Value>&);
        RESULT_FLAG intermediate_node_add(const std::pair<Key, Value>&);
	};
}
#endif

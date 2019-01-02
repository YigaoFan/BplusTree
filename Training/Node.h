// todo: what does the pragma mean?
#pragma once
#ifndef NODE_H
#define NODE_H

#include <vector> // for vector
#include <array> // for array
#include <memory> // for shared_ptr
#include <utility> // for pair
#include "NodeIter.h" // for NodeIter

namespace btree {

    template <typename Key, typename Value, typename Node_type>
    struct Ele {
        // todo: the key type should provide a default value to make
        // todo: default construct complete.
        union {
            // intermediate_node
            struct {
                Key child_value_low_bound;
                std::shared_ptr<Node_type> child;
            };
            // leaf
            struct {
                Key key;
                Value data;
            };
        };

        Ele(Key low_bound, std::shared_ptr<Node_type> node) : child_value_low_bound(low_bound), child(node) {}

        Ele(Key key, Value data) : key(key), data(data) {}

        Ele(const Ele &e);

        Ele &operator=(const Ele &e);

        // todo: the de-constructor maybe need to update
        ~Ele();

        // todo: + 1 is right?
        Ele* next() { return this + 1;}
    };

	template<typename Key, typename Value, unsigned BtreeOrder>
	class Node {
	private:
	    using ele_instance_type = Ele<Key, Value, Node>;
		using node_category = enum { intermediate_node = 1, leaf, };
		const node_category type_;
	private:

	public:
		explicit Node(const node_category&);
		// copy from stack's Ele
		Node(const node_category&, const ele_instance_type&);
	~Node();
		int insert(std::shared_ptr<ele_instance_type>);
		std::shared_ptr<Node> next_brother = nullptr;

	private:
		std::vector<std::shared_ptr<ele_instance_type>>& get_vector_of_elements_ref();
		// has_child will ensure the node isn't leaf and elements_count_ isn't 0
		bool has_child() { return type_ != leaf && elements_count_ > 0; }
		bool is_full() { return elements_count_ >= BtreeOrder; }
		void do_insert(std::shared_ptr<ele_instance_type>);
		void exchange_the_biggest_ele_out(std::shared_ptr<ele_instance_type>);

	private:
		std::shared_ptr<Node> father_ = nullptr;

		// tool function
		// todo: make the method below like this createNewRoot(node1, node2...);
		static int createNewRoot(const std::shared_ptr<Node> &, const std::shared_ptr<ele_instance_type> &);
		static std::shared_ptr<ele_instance_type> constructToMakeItFitTheFatherInsert(std::shared_ptr<ele_instance_type>);

		//new refactor:
	public:
		// for constructing iterator
		// todo: the function below is error, this function is belong to iterator
		ele_instance_type operator*(ele_instance_type *);
		ele_instance_type *operator++(ele_instance_type *);
		NodeIter<ele_instance_type> begin();
		NodeIter<ele_instance_type> end();

		std::shared_ptr<ele_instance_type> operator[](Key);

		// for check
		bool is_leaf() const { return type_ == leaf; }

		// add ele
		int add(std::pair<Key, Value>);

	private:
		// todo: use nodeType should test not produce error
		// todo: ensure the ele arranged by value size Ascending
		// todo: why here use shared_ptr<Ele>?
		std::array<std::shared_ptr<ele_instance_type>, BtreeOrder> elements_;

		decltype(elements_.size()) elements_count_{0};
	};
}
#endif

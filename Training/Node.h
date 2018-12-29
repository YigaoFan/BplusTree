#pragma once
#ifndef NODE_H
#define NODE_H

#include <vector> // for vector
#include <array> // for array
#include <memory> // for shared_ptr
#include <utility> // for pair

// thing below could use template
// start from 1, ensure not int initialization 0 situation

template <typename Key, typename Value, unsigned BtreeOrder>
class Node {
private:
	struct Ele {
		// todo: the key type should provide a default value to make 
	    // todo: default construct complete.
		union {
			// intermediate_node
			struct {
				Key child_value_low_bound;
				std::shared_ptr<Node> child;
			};
			// leaf
			struct {
				Key key;
				Value data;
			};
		};

		Ele(Key low_bound, shared_ptr<Node> node) : child_value_low_bound(low_bound), child(node) {}
		Ele(Key key, Value data) : key(key), data(data) {}
		Ele(const Ele& e);
		Ele& operator=(const Ele& e);
		// todo: the de-constructor maybe need to update 
		~Ele();
	};

public:
    Node(const node_type);
	// copy from stack's Ele
    Node(const node_type, const Ele);
    ~Node();
	int insert(std::shared_ptr<Ele>);
	std::shared_ptr<Node> next_brother = nullptr;
private:
    std::vector<std::shared_ptr<Ele>>& get_vector_of_elements_ref();
	// has_child will ensure the node isn't leaf and elements_count_ isn't 0
	bool has_child() { return type_ != leaf && elements_count_ > 0; }
	bool is_full() { return elements_count_ >= BtreeOrder; }
	void do_insert(shared_ptr<Ele>);
	void exchange_the_biggest_ele_out(shared_ptr<Ele>);

private:
	std::shared_ptr<Node> father_ = nullptr;

	// tool function
	// todo: make the method below like this createNewRoot(node1, node2...);
	static int createNewRoot(const std::shared_ptr<Node>&, const std::shared_ptr<Ele>&);
	static std::shared_ptr<Ele> constructToMakeItFitTheFatherInsert(std::shared_ptr<Ele>);

    //new refactor:
public:
    // for constructing iterator
    Ele operator*(Ele*);
    Ele* operator++(Ele*);
    Ele* begin();
    Ele* end();

    std::shared_ptr<Ele> operator[](Key);

    // for check
    bool is_leaf() const { return type_ == leaf; }
    // add ele
    int add(std::pair<Key, Value>);

private:
    using node_type = enum { intermediate_node = 1, leaf, };
    const node_type type_;

    // todo: use nodeType should test not produce error
    // todo: ensure the ele arranged by value size Ascending
    // todo: why here use shared_ptr<Ele>?
    std::array<std::shared_ptr<Ele>, BtreeOrder> elements_;

    decltype(elements_.size()) elements_count_{ 0 };
};

#endif

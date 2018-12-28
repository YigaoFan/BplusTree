#pragma once
#ifndef NODE_H
#define NODE_H

#include <vector>
#include <array>
#include <functional>
#include <memory>
//using std::vector;
//using std::shared_ptr;
//using std::make_shared;

// thing below could use template
typedef enum { intermediate_node = 1, leaf, } NodeType;


template <typename Key, typename Value, unsigned BtreeOrder>
class Node {
private:
	struct Ele {
		// todo: the key type should provide a default value
		union {
			// intermediate_node
			struct {
				Key childValueLowBound;
				shared_ptr<Node> child;
			};
			// leaf
			struct {
				Key key;
				Value data;
			};
		};

		Ele(Key lowBound, shared_ptr<Node> node) : childValueLowBound(key), child(node) {}
		Ele(Key key, Value data) : key(key), data(data) {}
		Ele(const Ele& e);
		Ele& operator=(const Ele& e);
		// todo: the de-constructor maybe need to update 
		~Ele();
	};

public:
    // Node and Ele sharing the type
    // start from 1, ensure not int initialization 0 situation
	typedef std::function<bool(const Ele&)> PredicateFunc;
    Node(const NodeType);
	// copy from stack's Ele
    Node(const NodeType, const Ele);
    ~Node();
	int insert(std::shared_ptr<Ele>);
	std::shared_ptr<Node> next_brother = nullptr;
private:
    std::vector<std::shared_ptr<Ele>>& get_vector_of_elements_ref();
    std::shared_ptr<Node> giveMeTheWay(PredicateFunc func);
	// has_child will ensure the node isn't leaf and elesCount isn't 0
	bool has_child() { return type != leaf && elesCount > 0; }
	bool is_full() { return children_count_bound_ >= eles.size(); }
	void do_insert(shared_ptr<Ele>);
	void exchange_the_biggest_ele_out(shared_ptr<Ele>);

private:
	// todo: use nodeType should test not produce error
	// todo: ensure the ele arranged by value size Ascending
    std::array<shared_ptr<Ele>, BtreeOrder> eles;
	shared_ptr<Node> father = nullptr;
    decltype(eles.size()) elesCount = 0;
	// todo: future use should change (by template?)
	decltype(eles.size()) children_count_bound_ = 3;
    const NodeType type_;

	// tool function
	// todo: make the method below like this createNewRoot(node1, node2...);
	static int createNewRoot(const shared_ptr<Node>&, const shared_ptr<Ele>&);
	static shared_ptr<Ele> constructToMakeItFitTheFatherInsert(shared_ptr<Ele>);

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
};

#endif

#pragma once
#ifndef NODE_H
#define NODE_H

#include <vector>
#include <array>
#include <functional>
#include <memory>
using std::vector;
using std::shared_ptr;
using std::make_shared;

// thing below could use template
typedef enum { intermediate_node = 1, leaf, } NodeType;

typedef std::function<bool(const Ele&)> PredicateFunc;

template <typename Key, typename Value, typename Ele, unsigned BtreeOrder>
class Node {
protected:
	// Node and Ele sharing the type
	// start from 1, ensure not int initialization 0 situation
	NodeType type;
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
    Node(NodeType);
    Node(NodeType, const shared_ptr<Ele>&);
    ~Node();
    // return 0 means OK, or not is bad
	int insert(shared_ptr<Ele>);
private:
    vector<shared_ptr<Ele>>& getVectorOfElesRef();
    shared_ptr<Node> giveMeTheWay(PredicateFunc func);
	// hasChild will ensure the node isn't leaf and elesCount isn't 0
	bool hasChild() { return type != leaf && elesCount > 0; }
	bool isFull() { return childrenCountBound >= eles.size(); }
	void doInsert(shared_ptr<Ele>);
	void exchangeTheBiggestEleOut(shared_ptr<Ele>);

private:
	// todo: use nodeType should test not produce error
	// todo: ensure the ele arranged by value size Ascending
    std::array<shared_ptr<Ele>, BtreeOrder> eles;
    shared_ptr<Node> nextBrother = nullptr;
	shared_ptr<Node> father = nullptr;
    decltype(eles.size()) elesCount = 0;
	// todo: future use should change (by template?)
	decltype(eles.size()) childrenCountBound = 3;

	// tool function
	// todo: make the method below like this createNewRoot(node1, node2...);
	static int createNewRoot(const shared_ptr<Node>&, const shared_ptr<Ele>&);
	static shared_ptr<Ele> constructToMakeItFitTheFatherInsert(shared_ptr<Ele>);
};

#endif

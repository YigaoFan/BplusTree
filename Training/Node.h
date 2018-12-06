#pragma once
#ifndef NODE_H
#define NODE_H

#include <vector>
#include <functional>
#include <memory>
using std::vector;
using std::shared_ptr;

// thing below could use template
typedef int KeyType;
typedef char DataType;

typedef std::function<bool(const Ele&)> PredicateFunc;

class Node {
protected:
	// Node and Ele sharing the type
	// start from 1, ensure not int initialization 0 situation
	enum NodeType { intermediate_node = 1, leaf, } type;
private:
	struct Ele {
		// todo: the key type should provide a default value
		union {
			// intermediate_node
			struct {
				KeyType childValueLowBound;
				shared_ptr<Node> child;
			};
			// leaf
			struct {
				KeyType key;
				DataType data;
			};
		};

		Ele(KeyType lowBound, shared_ptr<Node> node) : childValueLowBound(key), child(node) {}
		Ele(KeyType key, DataType data) : key(key), data(data) {}
		Ele(const Ele& e);
		Ele& operator=(const Ele& e);
		// todo: the de-constructor maybe need to update 
		~Ele();
	};

public:
    Node(NodeType);
    ~Node();
	int insert(shared_ptr<Ele>);
private:
    vector<Ele>& getVectorOfEles();
    shared_ptr<Node> giveMeTheWay(PredicateFunc func);
	// hasChild will ensure the node isn't leaf and elesCount isn't 0
	bool hasChild() { return type != leaf && elesCount > 0; }
	bool isFull() { return childrenCountBound >= eles.size(); }
	int doInsert(shared_ptr<Ele>);
	void swapTheBiggestEleOut(shared_ptr<Ele>);


private:
    // todo: the value below could be customize in the future
	// todo: use nodeType should test not produce error
	// todo: the code below initial how many eles?
	// todo: ensure the ele arranged by value size Ascending
    vector<Ele> eles;
    shared_ptr<Node> nextBrother = nullptr;
	shared_ptr<Node> father = nullptr;
    decltype(eles.size()) elesCount = 0;
	// todo: future use should change (by template?)
	decltype(eles.size()) childrenCountBound = 3;
};

#endif

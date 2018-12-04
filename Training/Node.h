#pragma once
#ifndef NODE_H
#define NODE_H

#include <vector>
#include <functional>
#include <memory>
#include "Ele.h"
using std::vector;
using std::shared_ptr;

typedef std::function<bool(const Ele&)> PredicateFunc;
typedef enum { LEAF_NODE, INTERMEDIATE_NODE, } NodeType;

class Node {
public:
	const NodeType nodeType;
    Node(NodeType type);
    ~Node();
    vector<Ele>& getVectorOfEles();
    shared_ptr<Node> giveMeTheWay(PredicateFunc func);
	// hasChild will ensure the node isn't leaf and elesCount isn't 0
	bool hasChild() { return nodeType!=LEAF_NODE && elesCount > 0; }

private:
    // todo: the value below could be customize in the future
	// todo: use nodeType should test not produce error
	// todo: the code below initial how many eles?
	// todo: ensure the ele arranged by value size Ascending
    vector<Ele> eles/* = vector<Ele>(3, nodeType)*/;
    shared_ptr<Node> nextBrother = nullptr;
    decltype(eles.size()) elesCount = 0;
};

#endif

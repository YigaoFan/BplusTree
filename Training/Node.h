#pragma once
#ifndef NODE_H
#define NODE_H

#include <vector>
#include <functional>
using std::vector;
typedef int KeyType;
typedef char DataType;
class Node;
struct Ele;
typedef std::function<bool(const Ele&)> PredicateFunc;

typedef enum { LEAF_NODE, INTERMEDIATE_NODE, } NodeType;

struct Ele {
	// todo: use union to store data and Node?
	union {
		struct {
			// todo: the key type should provide a default value
			KeyType childValueLowBound;
			Node* child;
		} /*intermediate_node*/;
		struct {
			// todo: the key and data type should provide a default value
			KeyType key;
			DataType data;
		} /*leaf_node*/;
	} /*u*/;

	Ele(KeyType key) : childValueLowBound(key), child(nullptr) {}
	Ele(DataType data) : key(data), data(data) {}
};

class Node {
public:
	const NodeType nodeType;
    Node(NodeType type);
    ~Node();
    vector<Ele>& getVectorOfEles();
    Node* giveMeTheWay(PredicateFunc func);
	// hasChild will ensure the node isn't leaf and elesCount isn't 0
	bool hasChild() { return nodeType!=LEAF_NODE && elesCount > 0; }

private:
    // todo: the value below could be customize in the future
	// todo: use nodeType should test not produce error
	// todo: the code below initial how many eles?
	// todo: ensure the ele arranged by value size Ascending
    vector<Ele> eles/* = vector<Ele>(3, nodeType)*/;
    Node* nextBrother = nullptr;
    decltype(eles.size()) elesCount = 0;
};

#endif

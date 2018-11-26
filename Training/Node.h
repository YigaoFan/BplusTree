#pragma once
#ifndef NODE_H
#define NODE_H

#include <vector>
using std::vector;
typedef int KeyType;
typedef int DataType;
class Node;
struct Ele;
typedef bool(*PredicateFunc)(const Ele&);

typedef enum { LEAF, INTERMEDIATE_NODE, } EleType;

struct Ele {
	// todo: use union to store data and Node?
	union {
		struct {
			// todo: the key type should provide a default value
			KeyType childValueLowBound;
			Node* child = nullptr;
		} /*intermediate_node*/;
		struct {
			// todo: the key and data type should provide a default value
			KeyType key;
			DataType data;
		} /*leaf_node*/;
	} /*u*/;

	EleType eleType = INTERMEDIATE_NODE;

	Ele() {}
};

class Node {
public:
    Node() = default;
    ~Node();
    vector<Ele>& getVectorOfEles();
    Node* giveMeTheWay(PredicateFunc func);
	bool hasChild() { return elesCount > 0; }

private:
    // todo: the value below could be customize in the future
    vector<Ele> eles{(3)};
    Node* nextBrother = nullptr;
    decltype(eles.size()) elesCount = 0;
};

#endif

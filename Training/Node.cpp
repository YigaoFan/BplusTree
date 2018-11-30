//should use difference compilation 
#include "pch.h"
#include "Node.h"

Node::Node(NodeType type) :nodeType(type)
{
	
}
Node::~Node()
{
}

vector<Ele>& Node::getVectorOfEles()
{
    return eles;
}

// caller should ensure the node is not leaf node
shared_ptr<Node> Node::giveMeTheWay(PredicateFunc func)
{
	vector<Ele>& eles = getVectorOfEles();
	// once encounter a node meeting the demand, return
	for (auto& e : eles) {
		if (func(e)) {
			return e.child;
		}
	}
	return nullptr;
}
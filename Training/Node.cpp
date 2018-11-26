//should use difference compilation 
#include "pch.h"
#include "Node.h"

Node::~Node()
{
}

vector<Ele>& Node::getVectorOfEles()
{
    return eles;
}

Node* Node::giveMeTheWay(PredicateFunc func)
{
	vector<Ele>& eles = getVectorOfEles();
	// once encounter a node meeting the demand, return
	for (auto e : eles) {
		if (func(e)) {
			return e.child;
		}
	}
	return nullptr;
}
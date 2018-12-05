//should use difference compilation 
#include "pch.h"
#include "Node.h"

Node::Node(NodeType type) :type(type)
{
	
}
Node::~Node()
{
}

vector<Node::Ele>& Node::getVectorOfEles()
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

void Node::swapTheBiggestEleOut(shared_ptr<Ele> e)
{
	// also need to think the intermediate_node situation
	for (auto e : this->getVectorOfEles()) {
		// need to use iterator
		if (e.key > e.key) {

		}
	}
}

// external caller should ensure the node is leaf
int Node::insert(shared_ptr<Ele> e)
{
	if (!this->isFull()) {
		return this->doInsert(e);
	} else {
		this->swapTheBiggestEleOut(e);
		if (this->nextBrother != nullptr) {
			return this->nextBrother->insert(e);
		} else {
			if (this->father != nullptr) {
				// todo: need to package this leaf to a withe
				// todo: maybe lead the data structure to change
				return this->father->insert(e);
			} else {
				// create new branch
			}
		}
	}
}
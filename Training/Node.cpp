//should use difference compilation 
//#include "pch.h"
#include "Node.h"
#include <algorithm>

Node::Node(NodeType type) :type(type)
{

}

Node::Node(NodeType type, const shared_ptr<Ele>& e) : Node(type)
{
    this->eles.push_back(e);
}

Node::~Node()
{
}

vector<shared_ptr<Node::Ele>>&
Node::getVectorOfElesRef()
{
	return eles;
}

// caller should ensure the node is not leaf node
shared_ptr<Node>
        Node::giveMeTheWay(PredicateFunc func)
{
	auto eles = getVectorOfElesRef();
	// once encounter a node meeting the demand, return
	for (auto& e : eles) {
		if (func(e)) {
			return e.child;
		}
	}
	return nullptr;
}

// the below function's scope is leaf
void
Node::exchangeTheBiggestEleOut(shared_ptr<Ele> e)
{
	// also need to think the intermediate_node situation
	// doesn't exist up situation todo: think it!
    this->doInsert(e);
    auto& eleVectorRef = this->getVectorOfElesRef();
	*e = *eleVectorRef.back();
	eleVectorRef.pop_back();
}

shared_ptr<Node::Ele> constructElePointingToNodePackagingThisEle
// external caller should ensure the insert node is leaf
int
Node::insert(shared_ptr<Ele> e)
{
	if (!this->isFull()) {
		// todo: doInsert need to attention to this Node is leaf or not
		this->doInsert(e);
		return 0;
	} else {
		this->exchangeTheBiggestEleOut(e);
		if (this->nextBrother != nullptr) {
			return this->nextBrother->insert(e);
		} else {
			// package this leaf to a withe
			shared_ptr<Ele> newEle = constructElePointingToNodePackagingThisEle(e);
			shared_ptr<Node> node = make_shared<Node>(e);
			// construct a Ele pointing to the node?
			if (this->father != nullptr) {
				// todo: maybe lead the data structure to change
				return this->father->insert(e);
			} else {
				// create new branch
				return createNewRoot(this->father);
			}
		}
	}
}


void
Node::doInsert(shared_ptr<Ele> e)
{
    auto& eleVectorRef = this->getVectorOfElesRef();
    eleVectorRef.push_back(e);
	std::sort(eleVectorRef.begin(), eleVectorRef.end(),
	        [] (shared_ptr<Ele> a, shared_ptr<Ele> b)
            {
	            return a->key < b->key;
            });
}

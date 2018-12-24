#include "Node.h"
#include <algorithm>
#define NODE_TEMPLATE_DECLARATION template <typename Key, typename Value, unsigned BtreeOrder>
#define NODE_INSTANCE Node<Key, Value, BtreeOrder>

NODE_TEMPLATE_DECLARATION
NODE_INSTANCE::Node(const NodeType type) :type_(type)
{

}

NODE_TEMPLATE_DECLARATION
NODE_INSTANCE::Node(const NodeType type, const Ele e) : Node(type)
{
    this->eles.push_back(e);
}

NODE_TEMPLATE_DECLARATION
NODE_INSTANCE::~Node()
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

shared_ptr<Node::Ele> 
Node::constructToMakeItFitTheFatherInsert(shared_ptr<Ele> e)
{
	// construct Ele Pointing To Node Packaging This Ele
	shared_ptr<Node>&& node = make_shared<Node>(intermediate_node, e);
	return make_shared<Ele>(e->key, node);
}

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
			shared_ptr<Ele> newEle = constructToMakeItFitTheFatherInsert(e);
			if (this->father != nullptr) {
				// todo: usually when move to father to insert Ele,
				// todo: it's the end of this level. Think it!
				return this->father->insert(newEle);
			} else {
				// create new branch
				return createNewRoot(this->father, newEle);
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

// todo: how to ensure only the root 
// todo: have and can trigger this method
int 
Node::createNewRoot(const shared_ptr<Node>& oldRoot, const shared_ptr<Ele>& risingEle)
{
	// todo: maybe the argument below is wrong, the keyType, not the NodeType
	shared_ptr<Ele>&& ele = make_shared<Ele>(/*intermediate_node*/, oldRoot);
	shared_ptr<Node>&& node = make_shared<Node>(/*intermediate_node*/, risingEle);

}

//for constructing iterator
NODE_TEMPLATE_DECLARATION
typename NODE_INSTANCE::Ele
NODE_INSTANCE::operator*(Ele* ele_ptr)
{
    return *ele_ptr;
}

NODE_TEMPLATE_DECLARATION
typename NODE_INSTANCE::Ele* 
NODE_INSTANCE::operator++(Ele* ele_ptr)
{
    return ++ele_ptr;
}

NODE_TEMPLATE_DECLARATION
typename NODE_INSTANCE::Ele*
NODE_INSTANCE::begin()
{
    return &eles[0];
}

NODE_TEMPLATE_DECLARATION
typename NODE_INSTANCE::Ele*
NODE_INSTANCE::end()
{
    return &eles[BtreeOrder];
}
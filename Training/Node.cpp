#include "Node.h"
// todo: sort used?
#include <algorithm> 
#include <utility>
//#include <memory.h> 

using namespace btree;
using std::shared_ptr;
using std::make_shared;
using std::vector;
using std::pair;
#define NODE_TEMPLATE_DECLARATION template <typename Key, typename Value, unsigned BtreeOrder>
#define NODE_INSTANCE Node<Key, Value, BtreeOrder>


// caller should ensure the node is not leaf node
//shared_ptr<Node>
//Node::giveMeTheWay(PredicateFunc func)
//{
//	auto eles = getVectorOfElesRef();
//	// once encounter a node meeting the demand, return
//	for (auto& e : eles) {
//		if (func(e)) {
//			return e.child;
//		}
//	}
//	return nullptr;
//}

// the below function's scope is leaf
// todo: function below could be used
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
RESULT_FLAG
Node::insert(shared_ptr<Ele> e)
{
	if (!this->isFull()) {
		// todo: do_insert need to attention to this Node is leaf or not
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
RESULT_FLAG
Node::createNewRoot(const shared_ptr<Node>& oldRoot, const shared_ptr<Ele>& risingEle)
{
	// todo: maybe the argument below is wrong, the keyType, not the node_category
	shared_ptr<Ele>&& ele = make_shared<Ele>(/*intermediate_node*/, oldRoot);
	shared_ptr<Node>&& node = make_shared<Node>(/*intermediate_node*/, risingEle);

}

// public method part:

NODE_TEMPLATE_DECLARATION
NODE_INSTANCE::Node(const node_category& type) :type_(type)
{

}

NODE_TEMPLATE_DECLARATION
NODE_INSTANCE::Node(const node_category& type, const ele_instance_type& e) : Node(type)
{
    this->elements_.push_back(e);
}

NODE_TEMPLATE_DECLARATION
NODE_INSTANCE::~Node()
{
}

//for constructing iterator
NODE_TEMPLATE_DECLARATION
NodeIter<typename NODE_INSTANCE::ele_instance_type>
NODE_INSTANCE::begin()
{
    return NodeIter<ele_instance_type>(&elements_[0]);
}

NODE_TEMPLATE_DECLARATION
NodeIter<typename NODE_INSTANCE::ele_instance_type>
NODE_INSTANCE::end()
{
    return NodeIter<ele_instance_type>(&elements_[elements_count_]);
}

/// may return nullptr when not found
NODE_TEMPLATE_DECLARATION
// I don't know exactly whether I could use ele_instance_type like this.
shared_ptr<typename NODE_INSTANCE::ele_instance_type>
NODE_INSTANCE::operator[](Key k)
{
    for (ele_instance_type& ele : *this) {
        if (ele->key == k) {
            // or could use return *ele directly? shared_ptr supported?
            return make_shared<ele_instance_type>(*ele);
        }
    }
    return nullptr;
}

// if node is a leaf, then add into leaf
// or it's a intermediate node, basically you can
// be sure to create a new node
NODE_TEMPLATE_DECLARATION
RESULT_FLAG
NODE_INSTANCE::add(const pair<Key, Value>& pair)
{
    // Fan said: We focus the operation of so many data structure on one abstract data structure

    // Because only one leaf node is generated in the Btree, 
    // all the nodes are actually generated here.
    if (this->is_leaf()) {
        // add Ele, there is only one situation--
        if (this->is_full()) {
            return this->leaf_full_then_add(pair);
        } else {
            return this->leaf_has_area_add(pair);
        }
    } else {
        return this->intermediate_node_add(pair);
    }
}

/// for the upper level Btree::remove, so
NODE_TEMPLATE_DECLARATION
void
NODE_INSTANCE::remove(const Key& key)
{
    if (this->is_leaf()) {
        if ((*this)[key] != nullptr) {
            // do some memory copy
            --(this->elements_count_);
            return;
        } else {
            // key isn't exist in this Node
            return;
        }
    } else {
        // when not a leaf-node, no need to remove. 
        // the Btree::check_out ensure the correctness.
        return;
    }
}

// private method part:

NODE_TEMPLATE_DECLARATION
RESULT_FLAG
NODE_INSTANCE::leaf_full_then_add(const pair<Key, Value>& pair)
{
    
}

NODE_TEMPLATE_DECLARATION
RESULT_FLAG
NODE_INSTANCE::leaf_has_area_add(const pair<Key, Value>& pair)
{
    // for (ele_instance_type& e : *this) {
    for (auto iter = this->begin(); iter != this->end(); ++iter) {
        // todo: implement how to get compare
        // once the pair.key < e.key, arrive the position
        if (compare(pair.first, iter->key)) {
            // todo: memory back shift, there may be problems
            // todo: , because relate to object function pointer address
            // todo: maybe not correct
            memcpy(iter->address() + 1, iter->address(), this->end() - iter);
            iter->key = pair.first;
            iter->data = pair.second;
            break;
        }
    }
    this->end()->key = pair.first;
    this->end()->data = pair.second;
    ++(this->elements_count_);
}

NODE_TEMPLATE_DECLARATION
RESULT_FLAG
NODE_INSTANCE::intermediate_node_add(const pair<Key, Value>& pair)
{
    
}


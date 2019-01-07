#include <utility>
#include "Node.h"

using namespace btree;
using std::shared_ptr;
using std::make_shared;
using std::pair;
#define NODE_TEMPLATE_DECLARATION template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
#define NODE_INSTANCE Node<Key, Value, BtreeOrder, BtreeType>

// the below function's scope is leaf
//void
//Node::exchangeTheBiggestEleOut(shared_ptr<Ele> e)
//{
//	// also need to think the intermediate_node situation
//	// doesn't exist up situation todo: think it!
//    this->doInsert(e);
//    auto& eleVectorRef = this->getVectorOfElesRef();
//	*e = *eleVectorRef.back();
//	eleVectorRef.pop_back();
//}
//
//shared_ptr<Node::Ele>
//Node::constructToMakeItFitTheFatherInsert(shared_ptr<Ele> e)
//{
//	// construct Ele Pointing To Node Packaging This Ele
//	shared_ptr<Node>&& node = make_shared<Node>(intermediate_node, e);
//	return make_shared<Ele>(e->key, node);
//}
//
//// external caller should ensure the insert node is leaf
//RESULT_FLAG
//Node::insert(shared_ptr<Ele> e)
//{
//	if (!this->isFull()) {
//		// todo: do_insert need to attention to this Node is leaf or not
//		this->doInsert(e);
//		return 0;
//	} else {
//		this->exchangeTheBiggestEleOut(e);
//		if (this->nextBrother != nullptr) {
//			return this->nextBrother->insert(e);
//		} else {
//			// package this leaf to a withe
//			shared_ptr<Ele> newEle = constructToMakeItFitTheFatherInsert(e);
//			if (this->father != nullptr) {
//				// todo: usually when move to father to insert Ele,
//				// todo: it's the end of this level. Think it!
//				return this->father->insert(newEle);
//			} else {
//				// create new branch
//				return createNewRoot(this->father, newEle);
//			}
//		}
//	}
//}
//
//
//void
//Node::doInsert(shared_ptr<Ele> e)
//{
//    auto& eleVectorRef = this->getVectorOfElesRef();
//    eleVectorRef.push_back(e);
//	std::sort(eleVectorRef.begin(), eleVectorRef.end(),
//	        [] (shared_ptr<Ele> a, shared_ptr<Ele> b)
//            {
//	            return a->key < b->key;
//            });
//}
//
//// todo: how to ensure only the root 
//// todo: have and can trigger this method
//RESULT_FLAG
//Node::createNewRoot(const shared_ptr<Node>& oldRoot, const shared_ptr<Ele>& risingEle)
//{
//	// todo: maybe the argument below is wrong, the keyType, not the bool_
//	shared_ptr<Ele>&& ele = make_shared<Ele>(/*intermediate_node*/, oldRoot);
//	shared_ptr<Node>&& node = make_shared<Node>(/*intermediate_node*/, risingEle);
//
//}

// public method part:

NODE_TEMPLATE_DECLARATION
NODE_INSTANCE::Node(const bool& middle, const BtreeType* btree, const Node* father) 
:middle(middle), btree_(btree), father_(father)
{

}

NODE_TEMPLATE_DECLARATION
NODE_INSTANCE::Node(const bool& middle, const BtreeType* btree, const pair<Key, Value>& pair, const Node* father) 
: Node(middle, btree, father)
{
    // todo: process the pair
    //this->elements_.push_back(e);
}

NODE_TEMPLATE_DECLARATION
NODE_INSTANCE::~Node()
{
}

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
shared_ptr<typename NODE_INSTANCE::ele_instance_type>
NODE_INSTANCE::operator[](const Key&& k)
{
    for (ele_instance_type& e : *this) {
        if (e->key == k) {
            return make_shared<ele_instance_type>(e);
        }
    }
    return nullptr;
}

NODE_TEMPLATE_DECLARATION
RESULT_FLAG
NODE_INSTANCE::add(const pair<Key, Value>& pair)
{
    // if node is a leaf, then add into leaf
    // or it's a intermediate node, basically you can
    // be sure to create a new node
    // Fan said: We focus the operation of so many data structure on one abstract data structure
    // all the nodes are actually generated here.

    if (this->middle) {
        return this->middle_node_add(pair);
    } else {
        shared_ptr<ele_instance_type> tmp = this->operator[](pair.first);
        // check existed
        if (tmp != nullptr) {
            // involve Ele detail, but the pair has meant detail info
            tmp->leaf.second = pair.second;
            return OK;
        } else {
            // so key is smaller than all Node's keys certainly
            if (this->full()) {
                return this->no_area_add(pair);
            } else {
                return this->area_add(pair);
            }
        }
        
    }
}

/// for the upper level Btree::remove, so
NODE_TEMPLATE_DECLARATION
void
NODE_INSTANCE::remove(const Key& key)
{
    if (this->middle) {
        // when not a leaf-node, no need to remove. 
        // the Btree::check_out ensure the correctness.
        return;
    } else {
        if (this->operator[](key) != nullptr) {
            // do some memory copy
            --(this->elements_count_);
            return;
        } else {
            // key isn't exist in this Node
            return;
        }
    }
}

// private method part:

NODE_TEMPLATE_DECLARATION
RESULT_FLAG
NODE_INSTANCE::no_area_add(pair<Key, Value> pair)
{
    NodeIter<ele_instance_type> end = this->end();
    // todo: care here is rvalue reference and modify other place
    for (NodeIter<ele_instance_type>&& iter = this->begin(); iter != end; ++iter) {
        if (btree_->compare_func_(pair.first, iter->key())) {
            ele_instance_type copy = *end;
            this->move_Ele(iter, end - 1);
            iter->leaf = pair;
            // todo: call another way to process the temp ele_instance_type
        }
    }
}

NODE_TEMPLATE_DECLARATION
RESULT_FLAG
NODE_INSTANCE::area_add(const pair<Key, Value>& pair)
{
    NodeIter<ele_instance_type> end = this->end();

    for (NodeIter<ele_instance_type> iter = this->begin(); iter != end; ++iter) {
        // once the pair.key < e.key, arrive the insert position
        if (btree_->compare_func_(pair.first, iter->key())) {
            this->move_Ele(iter, this->end());
            iter->key() = pair.first;
            iter->value() = pair.second;
            ++(this->elements_count_);
            return OK;
        }
    }
}

NODE_TEMPLATE_DECLARATION
RESULT_FLAG
NODE_INSTANCE::middle_node_add(const pair<Key, Value>& pair)
{
    
}

NODE_TEMPLATE_DECLARATION
void
NODE_INSTANCE::move_Ele(const NodeIter<ele_instance_type>& begin,
const NodeIter<ele_instance_type>& end, unsigned distance)
{
    //  memory back shift, there may be problems
    // , because relate to object function pointer address maybe not correct
    ele_instance_type* src = begin.operator->();
    const size_t len = (end - begin) * sizeof(NodeIter<ele_instance_type>::value_type);
    memcpy(src + distance, src, len);
}

NODE_TEMPLATE_DECLARATION
void
NODE_INSTANCE::adjust()
{
    
}
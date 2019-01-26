#include <utility>
#include "Node.h"
using namespace btree;
using std::pair;
using std::make_pair;
using std::vector;
#define NODE_TEMPLATE template <typename Key, typename Value, unsigned BtreeOrder, typename BtreeType>
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

NODE_TEMPLATE
template <typename Iterator>
NODE_INSTANCE::Node(const BtreeType* btree, const leaf_type nul, Iterator begin, Iterator end,const Node* father)
    : middle(false), btree_(btree), father_(father), elements_(this, begin, end)
{
    // null
}

NODE_TEMPLATE
template <typename Iterator>
NODE_INSTANCE::Node(const BtreeType* btree, const middle_type nul, Iterator begin, Iterator end,const Node* father)
        : middle(true), btree_(btree), father_(father), elements_(this, begin, end)
{
    // null
}

NODE_TEMPLATE
NODE_INSTANCE::~Node() = default;

NODE_TEMPLATE
bool
NODE_INSTANCE::have(const Key& k)
{
    return elements_.have(k);
}

/// Btree should use have() check or other me to ensure existing
NODE_TEMPLATE
Value&
NODE_INSTANCE::operator[](const Key& k)
{
    return elements_[k];
}

NODE_TEMPLATE
RESULT_FLAG
NODE_INSTANCE::add(const pair<Key, Value>& pair)
{
    // if node is a leaf, then append into leaf
    // or it's a intermediate node, basically you can
    // be sure to create a new node
    // Fan said: We focus the operation of so many data structure on one abstract data structure

    if (middle) {
        // means here root_ add
        return this->middle_add(pair);
    } else {
        elements_.append(pair);
        return OK;
        // should distinguish full or not full?
        // so key is smaller than all Node's keys certainly
        if (this->full()) {
            return this->no_area_add(pair);
        } else {
            return this->area_add(pair);
        }

        
    }
}

/// for the upper level Btree::remove, so
NODE_TEMPLATE
void
NODE_INSTANCE::remove(const Key& key)
{
    if (!middle) {
        elements_.remove(key);
    }
    // when not a leaf-node, no need to remove. 
}

NODE_TEMPLATE
vector<Key> 
NODE_INSTANCE::all_key() const
{
    return elements_.all_key();
}

NODE_TEMPLATE
Key 
NODE_INSTANCE::max_key() const
{
    return elements_.max_key();
}

NODE_TEMPLATE
NODE_INSTANCE*
NODE_INSTANCE::min_value() const
{
    return elements_.ptr_of_min();
}

// private method part:

NODE_TEMPLATE
bool 
NODE_INSTANCE::full()
{
    return elements_.full();
}

NODE_TEMPLATE
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

NODE_TEMPLATE
RESULT_FLAG
NODE_INSTANCE::area_add(const pair<Key, Value>& pair)
{
    NodeIter<ele_instance_type> end = this->end();

    for (NodeIter<ele_instance_type> iter = this->begin(); iter != end; ++iter) {
        // once the pair.key < e.key, arrive the insert position
        if (btree_->compare_func_(pair.first, iter->key())) {
            this->move_Ele(iter, this->end());
            iter->key() = pair.first;
            iter->__value() = pair.second;
            ++(this->elements_count_);
            return OK;
        }
    }
}

NODE_TEMPLATE
RESULT_FLAG
NODE_INSTANCE::middle_add(const pair<Key, Value>& pair)
{
    
}

//NODE_TEMPLATE
//void
//NODE_INSTANCE::move_Ele(const NodeIter<ele_instance_type>& begin,
//const NodeIter<ele_instance_type>& end, unsigned distance)
//{
//    //  memory back shift, there may be problems
//    // , because relate to object function pointer address maybe not correct
//    ele_instance_type* src = begin.operator->();
//    const size_t len = (end - begin) * sizeof(NodeIter<ele_instance_type>::value_type);
//    memcpy(src + distance, src, len);
//}

NODE_TEMPLATE
void
NODE_INSTANCE::adjust()
{
    
}
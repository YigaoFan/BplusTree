#include <utility>
#include "Node.h"
using namespace btree;
using std::pair;
using std::vector;
using std::unique_ptr;
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
    : middle(false), btree_(btree), father_(father), elements_(begin, end)
{
    // null
}

NODE_TEMPLATE
template <typename Iterator>
NODE_INSTANCE::Node(const BtreeType* btree, const middle_type nul, Iterator begin, Iterator end,const Node* father)
        : middle(true), btree_(btree), father_(father), elements_(begin, end)
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
void
NODE_INSTANCE::add(const pair<Key, Value>& pair)
{
    // TODO 
    // the logic maybe not need to like this
    // because the Btree has process a lot of thing
    //if (middle) {
    //    // means here root_ add
    //    this->middle_add(pair); // need to process the return value
    //} else {
        this->element_add(pair);
    //}
}

// append for middle node
NODE_TEMPLATE
void
NODE_INSTANCE::middle_append(const pair<Key, unique_ptr<Node>>& pair, const bool need_check_full)
{
    if (!need_check_full || !this->full()) {
        elements_.append(pair); // for Btree add
    } else {
        // TODO maybe need to set father and other related place
        auto n = new Node(this, middle_type(), &pair, &pair + 1);
        next_node_ = n;

        if (father_ == nullptr) {
            vector<std::pair<Key, unique_ptr<Node>>> sons{
                {btree_->root_->max_key(),btree_->root_},
                { pair.first, n},
            };

            // below is not very nice, because these operations are included by Btree
            auto new_root = new Node(btree_, sons.begin(), sons.end());
            btree_->root_ = nullptr;
            btree_->root_.reset(new_root);
        } else {
            father_->middle_append({ pair.first, n });
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
NODE_INSTANCE::min_leaf() const
{
    return elements_.ptr_of_min();
}

NODE_TEMPLATE
NODE_INSTANCE*
NODE_INSTANCE::max_leaf() const
{
    return elements_.ptr_of_max();
}

// private method part:

NODE_TEMPLATE
bool 
NODE_INSTANCE::full() const
{
    return elements_.full();
}

//NODE_TEMPLATE
//RESULT_FLAG
//NODE_INSTANCE::no_area_add(pair<Key, Value> pair)
//{
//    NodeIter<ele_instance_type> end = this->end();
//    // todo: care here is rvalue reference and modify other place
//    for (NodeIter<ele_instance_type>&& iter = this->begin(); iter != end; ++iter) {
//        if (btree_->compare_func_(pair.first, iter->key())) {
//            ele_instance_type copy = *end;
//            this->move_Ele(iter, end - 1);
//            iter->leaf = pair;
//            // todo: call another way to process the temp ele_instance_type
//        }
//    }
//}
//
//NODE_TEMPLATE
//RESULT_FLAG
//NODE_INSTANCE::area_add(const pair<Key, Value>& pair)
//{
//    NodeIter<ele_instance_type> end = this->end();
//
//    for (NodeIter<ele_instance_type> iter = this->begin(); iter != end; ++iter) {
//        // once the pair.key < e.key, arrive the insert position
//        if (btree_->compare_func_(pair.first, iter->key())) {
//            this->move_Ele(iter, this->end());
//            iter->key() = pair.first;
//            iter->__value() = pair.second;
//            ++(this->elements_count_);
//            return OK;
//        }
//    }
//}

NODE_TEMPLATE
void 
NODE_INSTANCE::element_add(const std::pair<Key, Value>&  pair)
{
    if (elements_.full()) {
        // add
        auto p = elements_.exchange_max_out(pair);
        // TODO not very clear to the adjust first, or process other related Node first
        btree_->change_bound_upwards(this, this->max_key());
        // next node add
        if (next_node_ != nullptr) {
            next_node_.element_add(p);
        } else {
            this->father_add(p);
        }
    } else {
        auto max_change = elements_.add(pair);
        if (max_change == true) {
            // call BtreeHelper
            btree_->change_bound_upwards(this, this->max_key());
        }
    }
}

NODE_TEMPLATE
void 
NODE_INSTANCE::father_add(const pair<Key, Value>& pair)
{
    if (father_ == nullptr) {

    } else {
        // create new leaf
        auto n = new Node(this, leaf_type(), &pair, &pair + 1);
        // set next_node_
        next_node_ = n;
        // let father add
        father_->middle_append({ pair.first, n }, true);
    }
}
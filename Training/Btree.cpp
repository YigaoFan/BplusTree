// todo: should in implemention file includes header file? 
#include <algorithm>
#include "Btree.h"
using namespace btree;
using std::initializer_list;
using std::pair;
using std::function;
using std::sort;
using std::shared_ptr;
using std::vector;
// seem like the template-type-para is not useful in this file
#define BTREE_TEMPLATE_DECLARATION template <typename Key, typename Value, unsigned BtreeOrder>
#define BTREE_INSTANCE Btree<Key, Value, BtreeOrder>

// public method part:

BTREE_TEMPLATE_DECLARATION
BTREE_INSTANCE::Btree(const initializer_list<pair<Key, Value>> pair_list, 
    compare_function_type compare)
: compare_function_(compare)
{
	for (auto& pair : pair_list) {
        this->add(pair);
	}
}

BTREE_TEMPLATE_DECLARATION
BTREE_INSTANCE::~Btree()
{

}

BTREE_TEMPLATE_DECLARATION
int
BTREE_INSTANCE::add(pair<Key, Value> e)
{
    shared_ptr<node_instance_type> node = check_out(pair.first);
    node->add(e);
    // if node is a leaf, then add into leaf
    // or it's a intermediate node, basically you can
    // be sure to create a new node
}

// private method part:
// todo: private method should declare its use

BTREE_TEMPLATE_DECLARATION
void
BTREE_INSTANCE::adjust()
{

}

/// search the key in Btree, return the node that terminated, 
/// maybe not find the key-corresponding one, but the related one.
/// save the search information
BTREE_TEMPLATE_DECLARATION
shared_ptr<typename BTREE_INSTANCE::node_instance_type>
BTREE_INSTANCE::check_out(const Key key)
{
    shared_ptr<node_instance_type> current_node = this->root_;

    if (current_node->is_leaf()) {
        return current_node;
    } else {
        return check_out_recur_helper(key, current_node);
    }
}

BTREE_TEMPLATE_DECLARATION
shared_ptr<typename BTREE_INSTANCE::node_instance_type> 
BTREE_INSTANCE::check_out_recur_helper(Key const key, shared_ptr<node_instance_type> node)
{
    // could think how to remove the key parameter after the first call
    shared_ptr<node_instance_type> current_node = node;
    if (current_node->is_leaf()) {
        return current_node;
    }

    for (auto && ele : *current_node) {
        if (compare_function_(key, ele.childValueLowBound)) {
            return check_out_recur_helper(key, ele.child);
        }
    }
    return current_node;
}


// todo: maybe useless
BTREE_TEMPLATE_DECLARATION
vector<Value>&
BTREE_INSTANCE::traverse_leaf(predicate predicate) 
{
	vector<Value> result;
	auto& first_node = this->get_smallest_leaf_back();
	for (auto& current_node = this->get_smallest_leaf_back(); 
		current_node->nextBrother != nullptr; 
		current_node = current_node->nextBrother) {
		if (predicate(current_node)) {
			result.push_back(current_node);
		}
	}
}

BTREE_TEMPLATE_DECLARATION
shared_ptr<NodeType>
BTREE_INSTANCE::get_smallest_leaf_back() 
{

}

// helper for add method
BTREE_TEMPLATE_DECLARATION
shared_ptr<NodeType>
BTREE_INSTANCE::getFitLeafBack(PredicateFunc func)
{
	// for-each Node to chose which should be continued
	shared_ptr<NodeType> currentNode;
	for (currentNode = root;
		currentNode->hasChild(); // here ensure not explore the leaf
		currentNode = currentNode->giveMeTheWay(func));
	// will get the leaf node
	return currentNode;
}

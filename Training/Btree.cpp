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
// seem like the type-para is not useful in this file
#define BTREE_TEMPLATE_DECLARATION template <typename Key, typename Value, unsigned BtreeOrder>
#define BTREE_INSTANCE Btree<Key, Value, BtreeOrder>

// public method part:

BTREE_TEMPLATE_DECLARATION
BTREE_INSTANCE::Btree(initializer_list<pair<Key, Value>> pair_list, 
    function<bool(Key, Key)> compare) : compare_function_(compare)
{
	for (auto& pair : pair_list) {
        auto node = check_out(pair.first);
		if (node.type != leaf) {
			this->add(pair);
		} else {
			this->modify(pair);
		}
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
	// expression below exist some problem need to be clear
	// the return value is the father?
	// using Currying to determine a argument
	shared_ptr<NodeType> parent = getFitLeafBack([e] (const Ele& ele) {
			if (ele.childValueLowBound > e) {
			return true;
			} else {
			return false;
			}});

	//todo: here I use the std::sort directly,
	//todo: in the future, update to use add sort
	auto& eles = parent->getVectorOfElesRef();

	eles.push_back(e);
	sort(eles.begin(), eles.end(),
			[](const Ele& ele1, const Ele& ele2)
			{
			if (ele1.key < ele2.key) {
			return true;
			} else {
			return false;
			}
			});

	return 1;
}

// private method part:
// todo: private method should declare its use

BTREE_TEMPLATE_DECLARATION
void
BTREE_INSTANCE::adjust()
{

}

// search the key in Btree, return the node that terminated, 
// maybe not find the key-corresponding one, but the related one.
BTREE_TEMPLATE_DECLARATION
shared_ptr<typename BTREE_INSTANCE::node_instance_type>
BTREE_INSTANCE::check_out(Key key)
{
    auto current_node = this->root_;

    if (current_node->type) {
        return current_node;
    } else {
        current_node = check_out_recur_helper(key, current_node);
        return current_node;
    }
    
}

BTREE_TEMPLATE_DECLARATION
shared_ptr<typename BTREE_INSTANCE::node_instance_type> 
BTREE_INSTANCE::check_out_recur_helper(Key key, node_instance_type node)
{
    // could think how to remove the key parameter after the first call
    auto current_node = node;
    if (current_node->type == leaf) {
        return current_node;
    }
    // run below step must be intermediate_node
    // compare key with ele.childValueLowBound
    // chose the way
    for (auto && ele : *current_node) {
        if (compare_function_(key, ele.childValueLowBound)) {
            current_node = check_out_recur_helper(key, ele.child);
            break;
        }
    }
    return current_node;
}


// todo: maybe useless
BTREE_TEMPLATE_DECLARATION
vector<Value>&
BTREE_INSTANCE::traverseLeaf(Predicate predicate) 
{
	vector<Value> result;
	auto& first_node = this->getSmallestLeafBack();
	for (auto& current_node = this->getSmallestLeafBack(); 
		current_node->nextBrother != nullptr; 
		current_node = current_node->nextBrother) {
		if (predicate(current_node)) {
			result.push_back(current_node);
		}
	}
}

BTREE_TEMPLATE_DECLARATION
shared_ptr<NodeType>
BTREE_INSTANCE::getSmallestLeafBack() 
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

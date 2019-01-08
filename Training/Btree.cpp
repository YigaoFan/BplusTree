// todo: should in implementation file includes header file? 
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
#define BTREE_TEMPLATE_DECLARATION template <typename Key, typename Value, unsigned BtreeOrder, typename Compare>
#define BTREE_INSTANCE Btree<Key, Value, BtreeOrder, Compare>

// public method part:

BTREE_TEMPLATE_DECLARATION
BTREE_INSTANCE::Btree(Compare& compare_function,
        const initializer_list<pair<Key, Value>>& pair_list)
: compare_func_(compare_function)
{
	for (auto& p : pair_list) {
        this->add(p);
	}
}

BTREE_TEMPLATE_DECLARATION
BTREE_INSTANCE::~Btree()
{

}

BTREE_TEMPLATE_DECLARATION
Value
BTREE_INSTANCE::search(const Key& key)
const
{
    shared_ptr<node_instance_type> node = this->check_out(key);
    if (node->middle) {
        // should ensure copy, not reference
        // but (*node)[key] is reference
        shared_ptr<typename node_instance_type::ele_instance_type> temp = (*node)[key];
        if (temp != nullptr) {
            // todo: I think here involving some Ele detail, should make a function
            return temp->data;
        } else {
            return nullptr;
        }
    } else {
        // todo: return nullptr or Value();
        return nullptr;
    }
}

BTREE_TEMPLATE_DECLARATION
RESULT_FLAG
BTREE_INSTANCE::add(const pair<Key, Value>& pair)
{
    shared_ptr<node_instance_type> node = this->check_out(pair.first);
    return node->add(pair);
}

BTREE_TEMPLATE_DECLARATION
RESULT_FLAG
BTREE_INSTANCE::modify(const pair<Key, Value>& pair)
{
    shared_ptr<node_instance_type> node = this->check_out(pair.first);
    (*node)[pair.first] = pair.second;
    return 1;
}

BTREE_TEMPLATE_DECLARATION
vector<Key>
BTREE_INSTANCE::explore()
const
{
    vector<Key> key_collection;

    this->traverse_leaf([key_collection](shared_ptr<node_instance_type> node)
    {
        for (auto& ele : *node) {
            key_collection.push_back(ele.key);
        }
        return false;
    });

    return key_collection;
}

BTREE_TEMPLATE_DECLARATION
void
BTREE_INSTANCE::remove(const Key& key)
{
    shared_ptr<node_instance_type> node = this->check_out(key);
    // todo: should implement the Node method remove
    node->remove(key);
}

// private method part:
// todo: private method should declare its use 


/// search the key in Btree, return the node that terminated, 
/// find the key-corresponding one, but the related one.
/// save the search information
BTREE_TEMPLATE_DECLARATION
shared_ptr<typename BTREE_INSTANCE::node_instance_type>
BTREE_INSTANCE::check_out(const Key& key)
{
    if (this->root_->middle) {
        return this->root_;
    } else {
        return check_out_recur(key, this->root_);
    }
}

BTREE_TEMPLATE_DECLARATION
shared_ptr<typename BTREE_INSTANCE::node_instance_type> 
BTREE_INSTANCE::check_out_recur(const Key& key, const shared_ptr<node_instance_type>& node)
{
    // could think how to remove the key parameter after the first call
    shared_ptr<node_instance_type> current = node;
    if (current->middle) {
        for (auto& e : *current) {
            // involved Ele detail
            if (compare_func_(key, e.child_value_low_bound)) {
                return check_out_recur(key, e.child);
            }
        }
        return current;
    } else {
        return current;
    }
}

/// operate on the true Node, not the copy
BTREE_TEMPLATE_DECLARATION
vector<typename BTREE_INSTANCE::node_instance_type>
BTREE_INSTANCE::traverse_leaf(const predicate& predicate) 
{
	vector<node_instance_type> result;
	for (shared_ptr<node_instance_type> current_node = this->smallest_leaf_back(); 
		current_node->next_brother != nullptr; 
		current_node = current_node->next_brother) {
        // todo: how to predicate a Node? Or should use Ele
		if (predicate(current_node)) {
			result.push_back(current_node);
		}
	}

    return result;
}

BTREE_TEMPLATE_DECLARATION
shared_ptr<typename BTREE_INSTANCE::node_instance_type>
BTREE_INSTANCE::smallest_leaf_back()
{
    shared_ptr<node_instance_type> current_node = this->root_;
    for (;
        current_node->middle && (*current_node)[0].child != nullptr;
        current_node = (*current_node)[0].child) { }

    return current_node;
}


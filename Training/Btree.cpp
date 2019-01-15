// todo: should in implementation file includes header file? 
#include <algorithm> // for sort
#include <cmath> // for ceil
#include <array>
#include "Btree.h"
using namespace btree;
using std::initializer_list;
using std::pair;
using std::function;
using std::sort;
using std::shared_ptr;
using std::vector;
using std::make_shared;
using std::ceil;
using std::array;
using std::sort;
#define BTREE_TEMPLATE_DECLARATION template <typename Key, typename Value, unsigned BtreeOrder, typename Compare>
#define BTREE_INSTANCE Btree<Key, Value, BtreeOrder, Compare>

// public method part:

BTREE_TEMPLATE_DECLARATION
template <unsigned NumOfArrayEle>
BTREE_INSTANCE::Btree(const Compare compare_function/* = Compare()*/, //TODO: how to use less<>
        array<pair<Key, Value>, NumOfArrayEle>& pair_array)
    : compare_func_(compare_function)
{
    sort(pair_array.begin(), pair_array.end(), [compare_function](pair<Key, Value> p1, pair<Key, Value>p2)
    {
        return compare_function(p1.first, p2.first);
    });
    // sort the pair_array first, then construct the tree
    if (pair_array.size() <= BtreeOrder) {
        this->root_ = make_shared<node_instance_type>(this, nullptr, leaf_type(), pair_array.begin(), pair_array.end());
        return;
    }
    array<shared_ptr<node_instance_type>, ceil(pair_array.size() / BtreeOrder)> all_leaf;
    auto i = 0;
    decltype(pair_array.begin()) tail;
    auto end = pair_array.end();
    for (auto head = pair_array.begin();
         end - head >= BtreeOrder;
         head = tail) {
        tail = head + BtreeOrder;
        // range include head to tail is willing to construct a Node
        auto leaf = make_shared<node_instance_type>(this, nullptr/*temporary*/,leaf_type(), head, tail);
        // then collect this
        all_leaf[i] = leaf;
        ++i;
    }
    auto& back = all_leaf.back();
    back = make_shared<node_instance_type>(this, nullptr, tail, end);

    // then use the all_leaf array recursive construct a tree
    this->constructUpperNode(all_leaf);
}

BTREE_TEMPLATE_DECLARATION
template <unsigned NodeCount>
// array elements count should be a template arg
void
BTREE_INSTANCE::constructUpperNode(array<shared_ptr<node_instance_type>, NodeCount>& all_node)
{
    // when the count is smaller than BtreeOrder, we can start construct root_ node.
    if (all_node.size() < BtreeOrder) {
        this->root_ = make_shared<node_instance_type>(this, nullptr, middle_type(), all_node.begin(), all_node.end());
        // below should be a function
        for (shared_ptr<node_instance_type>& n : all_node) {
            n->father_ = this->root_.get();
        }
        return;
    }

    // below should be a function
    array<shared_ptr<node_instance_type>, ceil((all_node.size() / BtreeOrder))> all_upper_node;
    auto i = 0;
    decltype(all_node.begin()) tail;
    auto end = all_node.end();
    for (auto head = all_node.begin();
         end - head >= BtreeOrder;
         head = tail) {
       tail = head + BtreeOrder;

       auto middle_node = make_shared<node_instance_type>(this, nullptr, middle_type(), head, tail);
       all_upper_node[i] = middle_node;
       for (decltype(all_node.begin()) h = head, t = tail; head != tail; ++head) {
           h->father_ = middle_node.get();
       }

       ++i;
    }
    auto& back = all_upper_node.back();
    back = make_shared<node_instance_type>(this, nullptr, middle_type(), tail, end);
    this->constructUpperNode(all_upper_node);
}

BTREE_TEMPLATE_DECLARATION
template <unsigned NumOfArrayEle>
BTREE_INSTANCE::Btree(const Compare compare_function/* = Compare()*/, //TODO: how to use less<>
    array<pair<Key, Value>, NumOfArrayEle>&& pair_list)
    : Btree(compare_function, pair_list)
{
    
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
        // (*current_node)[0].child should use Node child count to judge
        current_node->middle && (*current_node)[0].child != nullptr;
        current_node = (*current_node)[0].child) { }

    return current_node;
}


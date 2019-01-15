// todo: should in implementation file includes header file?
#include "Btree.h"
#include <algorithm>  // for sort
#include <array>
#include <cmath>  // for ceil
using namespace btree;
using std::array;
using std::ceil;
using std::function;
using std::initializer_list;
using std::make_shared;
using std::pair;
using std::shared_ptr;
using std::sort;
using std::vector;
#define BTREE_TEMPLATE_DECLARE \
  template <typename Key, typename Value, unsigned BtreeOrder, typename Compare>
#define BTREE_INSTANCE Btree<Key, Value, BtreeOrder, Compare>

// public method part:

/// recommend in constructor construct all Key-Value
BTREE_TEMPLATE_DECLARE
template <unsigned NumOfArrayEle>
BTREE_INSTANCE::Btree(const Compare compare_function /* = Compare()*/,  // TODO: how to use less<>
    array<pair<Key, Value>, NumOfArrayEle>& pair_array)
    : compare_func_(compare_function) 
{
    if constexpr (NumOfArrayEle == 0) {
        return;
    }

    // TODO: how to check the same Key
    // sort the pair_array first
    sort(pair_array.begin(), pair_array.end(),
        [compare_function](pair<Key, Value> p1, pair<Key, Value> p2)
    {
        return compare_function(p1.first, p2.first);
    });

    if constexpr (NumOfArrayEle < BtreeOrder) {
        this->root_ = make_shared<node_instance_type>(this, leaf_type(), pair_array.begin(), pair_array.end());
    } else {
        // notice: just provide a template arg
        this->helper<true>(pair_array);
    }
}

/// tool to set Node father pointer
template <typename T>
void
set_father(typename T::iterator begin, const typename T::iterator& end, void* father)
{
    for (; begin != end; ++begin) {
        begin->father = father;
    }
}

/// assume the nodes arg is sorted
BTREE_TEMPLATE_DECLARE
template <bool FirstFlag, typename ElementType, unsigned NodeCount>
void
BTREE_INSTANCE::helper(array<ElementType, NodeCount>& nodes)
{
    constexpr size_t upper_node_num = static_cast<const size_t>(ceil(NodeCount / BtreeOrder));
    array<ElementType, upper_node_num> all_upper_node; // store all leaf

    auto head = nodes.begin();
    auto end = nodes.end();
    auto i = 0;

    // then construct the leaf
    if constexpr (NodeCount > BtreeOrder) {
        auto tail = head + BtreeOrder;
        do {
            // use head to tail pairs to construct a Node, then collect it
            if constexpr (FirstFlag) {
                auto leaf = make_shared<node_instance_type>(this, leaf_type(), head, tail);
                all_upper_node[i] = leaf;
            } else {
                auto middle = make_shared<node_instance_type>(this, middle_type(), head, tail);
                all_upper_node[i] = middle;
                // set father
                set_father(head, tail, middle.get());
            }

            // update
            head = tail;
            tail += BtreeOrder;
            ++i;
        } while (end - head > BtreeOrder);
        // not include = to ensure have to remain a group, then below statement
        // could be run correctly
    }
    if constexpr (FirstFlag) {
        all_upper_node[i] = make_shared<node_instance_type>(this, leaf_type(), head, end);
    } else {
        all_upper_node[i] = make_shared<node_instance_type>(this, middle_type(), head, end);
    }

    if constexpr (upper_node_num <= BtreeOrder) {
        this->root_ = make_shared<node_instance_type>(this, middle_type(), all_upper_node.begin(), all_upper_node.end());
        set_father(all_upper_node.begin(), all_upper_node.end(), root_.get());
    } else {
        // construct a tree recursively
        this->helper<false>(all_upper_node);
    }
}

BTREE_TEMPLATE_DECLARE
template <unsigned NumOfArrayEle>
BTREE_INSTANCE::Btree(
    const Compare compare_function /* = Compare()*/,  // TODO: how to use less<>
    array<pair<Key, Value>, NumOfArrayEle>&& pair_list)
    : Btree(compare_function, pair_list) {}

BTREE_TEMPLATE_DECLARE
BTREE_INSTANCE::~Btree() {}

BTREE_TEMPLATE_DECLARE
Value BTREE_INSTANCE::search(const Key& key) const {
  shared_ptr<node_instance_type> node = this->check_out(key);
  if (node->middle) {
    // should ensure copy, not reference
    // but (*node)[key] is reference
    shared_ptr<typename node_instance_type::ele_instance_type> temp =
        (*node)[key];
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

BTREE_TEMPLATE_DECLARE
RESULT_FLAG
BTREE_INSTANCE::add(const pair<Key, Value>& pair) {
  if (root_ == nullptr) {
    // TODO:
    return OK;
  }
  shared_ptr<node_instance_type> node = this->check_out(pair.first);
  return node->add(pair);
}

BTREE_TEMPLATE_DECLARE
RESULT_FLAG
BTREE_INSTANCE::modify(const pair<Key, Value>& pair) {
  shared_ptr<node_instance_type> node = this->check_out(pair.first);
  (*node)[pair.first] = pair.second;
  return 1;
}

BTREE_TEMPLATE_DECLARE
vector<Key> BTREE_INSTANCE::explore() const {
  vector<Key> key_collection;

  this->traverse_leaf([key_collection](shared_ptr<node_instance_type> node) {
    for (auto& ele : *node) {
      key_collection.push_back(ele.key);
    }
    return false;
  });

  return key_collection;
}

BTREE_TEMPLATE_DECLARE
void BTREE_INSTANCE::remove(const Key& key) {
  shared_ptr<node_instance_type> node = this->check_out(key);
  // todo: should implement the Node method remove
  node->remove(key);
}

// private method part:
// todo: private method should declare its use

/// search the key in Btree, return the node that terminated,
/// find the key-corresponding one, but the related one.
/// save the search information
BTREE_TEMPLATE_DECLARE
shared_ptr<typename BTREE_INSTANCE::node_instance_type>
BTREE_INSTANCE::check_out(const Key& key) {
  if (this->root_->middle) {
    return this->root_;
  } else {
    return check_out_recur(key, this->root_);
  }
}

BTREE_TEMPLATE_DECLARE
shared_ptr<typename BTREE_INSTANCE::node_instance_type>
BTREE_INSTANCE::check_out_recur(const Key& key,
                                const shared_ptr<node_instance_type>& node) {
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
BTREE_TEMPLATE_DECLARE
vector<typename BTREE_INSTANCE::node_instance_type>
BTREE_INSTANCE::traverse_leaf(const predicate& predicate) {
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

BTREE_TEMPLATE_DECLARE
shared_ptr<typename BTREE_INSTANCE::node_instance_type>
BTREE_INSTANCE::smallest_leaf_back() {
  shared_ptr<node_instance_type> current_node = this->root_;
  for (;
       // (*current_node)[0].child should use Node child count to judge
       current_node->middle && (*current_node)[0].child != nullptr;
       current_node = (*current_node)[0].child) {
  }

  return current_node;
}

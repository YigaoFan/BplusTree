#include <algorithm>  // for sort
#include <array> // for array
#include <cmath>  // for ceil
#include <cassert>
#include "Btree.h"
using namespace btree;
using std::array;
using std::ceil;
using std::function;
using std::pair;
using std::sort;
using std::vector;
using std::copy;
using std::unique_ptr;
#define BTREE_TEMPLATE \
  template <typename Key, typename Value, unsigned BtreeOrder>
#define BTREE_INSTANCE Btree<Key, Value, BtreeOrder>

// all method in this level, 
// when call lower level function, must ensure the Key exist
// public method part:

/// recommend in constructor construct all Key-Value
/// user should ensure not include the same Key-Value
/// Because when construct a tree, you can see all Keys
/// When you add or modify, you can use have() function to check it
BTREE_TEMPLATE
template <unsigned NumOfArrayEle>
BTREE_INSTANCE::Btree(const compare& compare_function,
    array<pair<Key, Value>, NumOfArrayEle>& pair_array)
    : compare_func_(compare_function)
    // Prepare memory
//    leaf_block_(LeafMemory<Key, Value, 2>::produce_leaf_memory(NumOfArrayEle))
{
    if constexpr (NumOfArrayEle == 0) {
        return;
    }

    // TODO check&process the same Key
    key_num_ += NumOfArrayEle;

    // sort the pair_array
    sort(pair_array.begin(), pair_array.end(),
        [compare_function](pair<Key, Value> p1, pair<Key, Value> p2)
    {
        return compare_function(p1.first, p2.first);
    });

    if constexpr (NumOfArrayEle < BtreeOrder) {
        root_.reset(new node_instance_type(this, leaf_type(), pair_array.begin(), pair_array.end()));
    } else {
        this->helper<true>(pair_array);
    }
}

/// assume the nodes arg is sorted
BTREE_TEMPLATE
template <bool FirstFlag, typename ElementType, unsigned NodeCount>
void
BTREE_INSTANCE::helper(const array<ElementType, NodeCount>& nodes)
{
    // ceil is for double, maybe need to be changed to int
    constexpr auto upper_node_num = static_cast<const size_t>(ceil(NodeCount / BtreeOrder));
    array<pair<Key, unique_ptr<node_instance_type>>, upper_node_num> all_upper_node; // store all leaf

    auto head = nodes.begin();
    auto end = nodes.end();
    auto i = 0;
    auto tail = head + BtreeOrder;
    auto not_first_of_arr = false;

    do {
        // use head to tail to construct a upper Node, then collect it
        if constexpr (FirstFlag) {
            auto leaf = new node_instance_type(this, leaf_type(), head, tail);

            // TODO complete
            assert(i < upper_node_num);

            all_upper_node[i] = { leaf->max_key(), unique_ptr<node_instance_type>(leaf) };
        } else {
            auto middle = new node_instance_type(this, middle_type(), head, tail);
            all_upper_node[i] = { middle->max_key(), unique_ptr<node_instance_type>(middle) };
            // set Node.next_node_
            if (not_first_of_arr) {
                all_upper_node[i].second->next_node_ = all_upper_node[i-1].second.get();
            } else {
                not_first_of_arr = true;
            }
        }

        // update
        head = tail;
        tail += BtreeOrder;
        ++i;
    } while (end - head > 0);

    if constexpr (upper_node_num <= BtreeOrder) {
        root_.reset(new node_instance_type(this, middle_type(), all_upper_node.begin(), all_upper_node.end()));
    } else {
        // construct recursively
        this->helper<false>(all_upper_node);
    }
}

BTREE_TEMPLATE
template <unsigned NumOfArrayEle>
BTREE_INSTANCE::Btree(
    const compare& compare_function,
    array<pair<Key, Value>, NumOfArrayEle>&& pair_array)
    : Btree(compare_function, pair_array) {}

BTREE_TEMPLATE
BTREE_INSTANCE::~Btree() = default;

BTREE_TEMPLATE
Value
BTREE_INSTANCE::search(const Key& key) const
{
    node_instance_type*&& node = this->check_out(key);
    if (node->middle) {
        // Value type should provide default constructor to represent null
        return Value(); // undefined behavior
    }
    return node->operator[](key); 
}

/// if exist, will modify
BTREE_TEMPLATE
RESULT_FLAG
BTREE_INSTANCE::add(const pair<Key, Value>& pair) {
    if (root_ == nullptr) {
        root_ = make_shared<node_instance_type>(this, nullptr, leaf_type(), &pair, &pair+1);
        return OK;
    }

    auto& k = pair.first;
    auto& v = pair.second;
    // TODO: the code below should be a function? and the code in modify
    node_instance_type*&& node = this->check_out(k);
    if (!node->middle) {
        if (node->have(k)) {
            // modify
            node->operator[](k) = v;
        } else {
            // add
            node->add(pair);
            ++key_num_;
        }
    } else {
        // add
        ++key_num_;
        node->add(pair);
    }
    return OK;
}

/// if not exist, will add
BTREE_TEMPLATE
RESULT_FLAG
BTREE_INSTANCE::modify(const pair<Key, Value>& pair) {
    Key& k = pair.first;
    Value& v = pair.second;
    node_instance_type*&& node = this->check_out(k);
    if (!node->middle) {
        // TODO: should think let the if logic below work by Node-self
        if (node->have(k)) {
            // modify
            node->operator[](k) = v;
        } else {
            // add
            node->add(pair);
            ++key_num_;
        }
    }
    node->add(pair);
    ++key_num_;

    return OK;
}

BTREE_TEMPLATE
vector<Key>
BTREE_INSTANCE::explore() const {
  vector<Key> k_array(key_num_);
  this->traverse_leaf([&k_array](node_instance_type* n) {
      static auto iter = k_array.begin();
      vector<Key>&& ks = n->all_key();
      iter = copy(ks.begin(), ks.end(), iter);
      return false;
  });

  return k_array;
}

BTREE_TEMPLATE
void
BTREE_INSTANCE::remove(const Key& key) {
    node_instance_type*&& n = this->check_out(key);
    n->remove(key); // TODO: whether to check if key exist
    --key_num_;
}

BTREE_TEMPLATE
bool
BTREE_INSTANCE::have(const Key& key)
{
    node_instance_type*&& r = this->check_out(key);
    return !(r->middle || !r->have(key));
}

// private method part:

/// search the key in Btree, return the node that terminated, may be not have the key
/// this is to save the search information
BTREE_TEMPLATE
typename BTREE_INSTANCE::node_instance_type*
BTREE_INSTANCE::check_out(const Key& key) {
  if (!root_->middle) {
    return root_;
  } else {
    return check_out_recur(key, root_);
  }
}

BTREE_TEMPLATE
typename BTREE_INSTANCE::node_instance_type*
BTREE_INSTANCE::check_out_recur(const Key& key, const node_instance_type* node) {
  if (node->middle) {
      if (node->have(key)) {
          check_out_recur(key, node->operator[](key));
      }
    return node;
  } else {
    return node;
  }
}

/// operate on the true Node
BTREE_TEMPLATE
vector<typename BTREE_INSTANCE::node_instance_type*>
BTREE_INSTANCE::traverse_leaf(const predicate& predicate) {
    vector<node_instance_type*> result;

    node_instance_type* current = this->smallest_leaf();
    do {
        if (predicate(current)) {
            result.push_back(current);
        }
        // update
        current = current->next_node_;
    } while (current != nullptr);

    return result;
}

BTREE_TEMPLATE
typename BTREE_INSTANCE::node_instance_type*
BTREE_INSTANCE::smallest_leaf() {
    node_instance_type* current_node = root_.get();

    while (current_node->middle) {
        current_node = current_node->ptr_of_min();
    }
   
    return current_node;
}

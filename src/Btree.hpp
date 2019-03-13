#pragma once

#include <vector> // for vector
#include <functional> // for lambda & less
#include <memory> // for shared_ptr, allocator
#include <utility> // for pair
#include "CommonFlag.hpp"
#include "BtreeHelper.hpp"
#include "Node.hpp"
//#include "NodeBase.hpp"

#ifdef BTREE_DEBUG
#include <iostream>
#include "utility.hpp"
#endif

// declaration
namespace btree {
	template<typename Key,
			 typename Value,
			 unsigned BtreeOrder>
	class Btree : private BtreeHelper {
	private:
		using node_instance_type = Node<Key, Value, BtreeOrder, Btree>;

	public:
		using compare = std::function<bool(Key, Key)>;
		const compare compare_func_;

		template <std::size_t NumOfArrayEle>
		Btree(const compare&, std::array<std::pair<Key, Value>, NumOfArrayEle>);
		template <unsigned long NumOfArrayEle>
		Btree(compare&&, std::array<std::pair<Key, Value>, NumOfArrayEle>&&);
		Btree(const Btree&); // copy constructor
		Btree(Btree&&); // move constructor
		~Btree() override = default;

		Btree& operator=(const Btree&); // copy assign
		Btree& operator=(Btree&&); // move assign

		Value search(const Key&) const;
		RESULT_FLAG add(const std::pair<Key, Value>&);
		RESULT_FLAG modify(const std::pair<Key, Value>&);
		std::vector<Key> explore() const;
		void remove(const Key&);
		bool have(const Key&) const;

	private:
		unsigned key_num_ = 0;

		using predicate = std::function<bool(node_instance_type*)>;
		std::unique_ptr<node_instance_type> root_{nullptr};


		node_instance_type* check_out(const Key&) const;
		static node_instance_type* check_out_digging(const Key&, node_instance_type*);
		std::vector<node_instance_type*> traverse_leaf(const predicate&) const;
		node_instance_type* smallest_leaf() const;
		node_instance_type* biggest_leaf();
		//node_instance_type* extreme_leaf(std::function<node_instance_type*()>);

		template <bool FirstFlag, typename Element, std::size_t NodeCount>
		void helper(const std::array<Element, NodeCount>&);

		//void root_add(const node_instance_type*, const std::pair<Key, Value>&);
		//void create_new_branch(const node_instance_type*, const std::pair<Key, Value>&);
		//void create_new_root(const node_instance_type*, const std::pair<Key, Value>&);
		void merge_branch(const Key, const node_instance_type*); // for Node merge a branch
	};
}

#include <algorithm>  // for sort
#include <array> // for array
#include <cmath>  // for ceil
#include <exception> // for exception
#include <iostream>

#ifdef BTREE_DEBUG
#include <cassert>
#endif

// implementation
namespace btree {
	using std::array;
	using std::function;
	using std::pair;
	using std::sort;
	using std::vector;
	using std::copy;
	using std::unique_ptr;
	using std::make_pair;
	using std::runtime_error;
	using std::allocator;

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
		template <std::size_t NumOfArrayEle>
	BTREE_INSTANCE::Btree(const compare& compare_function, array<pair<Key, Value>, NumOfArrayEle> pair_array)
		: BtreeHelper(), compare_func_(compare_function)
	{
		if constexpr (NumOfArrayEle == 0) {
			return;
		}

		// sort
		sort(pair_array.begin(), pair_array.end(),
			[&](const pair<Key, Value>& p1, const pair<Key, Value>& p2) {
			return compare_func_(p1.first, p2.first);
		});

		// duplicate check
		auto last = pair_array[0].first;
		for (auto beg = pair_array.begin() + 1, end = pair_array.end(); beg != end; ++beg) {
			if (beg->first == last) {
				throw runtime_error("The input array has the duplicate key");
			} else {
				last = beg->first;
			}
		}

		// construct
		if constexpr (NumOfArrayEle <= BtreeOrder) {
			root_.reset(new node_instance_type(this, leaf_type(), pair_array.begin(), pair_array.end()));
		} else {
			this->helper<true>(pair_array);
		}

		key_num_ += NumOfArrayEle;
	}

	BTREE_TEMPLATE
		BTREE_INSTANCE::Btree(const Btree& that)
		: BtreeHelper(that), key_num_(that.key_num_), root_(that.root_), compare_func_(that.compare_func_)
	{

	}

	BTREE_TEMPLATE
		BTREE_INSTANCE::Btree(Btree&& that)
		: BtreeHelper(that), key_num_(that.key_num_), root_(that.root_), compare_func_(that.compare_func_)
	{

	}

	BTREE_TEMPLATE
	BTREE_INSTANCE&
		BTREE_INSTANCE::operator=(const Btree& that)
	{

	}

	BTREE_TEMPLATE
	BTREE_INSTANCE&
		BTREE_INSTANCE::operator=(Btree&& that)
	{

	}

	BTREE_TEMPLATE
		template <bool FirstCall, typename ElementType, std::size_t NodeCount>
	void
		BTREE_INSTANCE::helper(const array<ElementType, NodeCount>& nodes)
	{
		constexpr auto upper_node_num = (NodeCount % BtreeOrder == 0) ? (NodeCount / BtreeOrder) : (NodeCount / BtreeOrder + 1);
		// store all nodes
		array<pair<Key, node_instance_type*>, upper_node_num> all_upper_node;

		auto head = nodes.begin();
		auto end = nodes.end();
		auto i = 0;
		auto tail = head + BtreeOrder;
		auto first = true;

		do {
			if constexpr (FirstCall) {
				auto leaf = new node_instance_type(this, leaf_type(), head, tail);

#ifdef BTREE_DEBUG
				LOG(tail - head);
				for (auto b = head; b != tail; ++b) {
					static int count = 0;
					LOG(b->first);
					++count;
					LOG(count);
				}
#endif
				// defense code
				assert(i < upper_node_num);

				all_upper_node[i] = { leaf->max_key(), leaf };
				if (first) {
					first = false;
				} else {
					// set next_node of Node
					all_upper_node[i - 1].second->next_node(leaf);
				}
			} else {
				auto middle = new node_instance_type(this, middle_type(), head, tail);

#ifdef BTREE_DEBUG
				for (auto b = head; b != tail; ++b) {
					static char count = 0;
					LOG(b->first);
					++count;
				}
#endif

				//defense code
				assert(i < upper_node_num);

				all_upper_node[i] = { middle->max_key(), middle };
				if (first) {
					first = false;
				} else {
					// set next_node of Node
					all_upper_node[i - 1].second->next_node(middle);
				}
			}

			// update
			head = tail;
			tail = (end - tail > BtreeOrder) ? (end + BtreeOrder) : end;
			++i;

		} while (end - head > 0);

		if constexpr (upper_node_num <= BtreeOrder) {
			root_.reset(new node_instance_type(this, middle_type(), all_upper_node.begin(), all_upper_node.end()));
		} else {
			// recursive
			this->helper<false>(all_upper_node);
		}
	}

	// TODO wait test
	BTREE_TEMPLATE
	template <unsigned long NumOfArrayEle>
	BTREE_INSTANCE::Btree(compare&& compare_function, array<pair<Key, Value>, NumOfArrayEle>&& pair_array)
	    : Btree(compare_function, pair_array) {}

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
		BTREE_INSTANCE::add(const pair<Key, Value>& pair)
	{
		if (root_ == nullptr) {
			root_.reset(new node_instance_type(this, nullptr, leaf_type(), &pair, &pair + 1));
			++key_num_;
			return OK;
		}

		auto& k = pair.first;
		auto& v = pair.second;
		// TODO: the code below should be a function? and the code in modify
		node_instance_type*&& node = this->check_out(k);
		if (!node->middle) {
			if (node->have(k)) {
				node->operator[](k) = v; // modify
			} else {
				node->add(pair);
				++key_num_;
			}
		} else {
			auto leaf = this->biggest_leaf();
			leaf->add(pair);

			++key_num_;
		}
		return OK;
	}

	//BTREE_TEMPLATE
	//void
	//BTREE_INSTANCE::root_add(const node_instance_type* middle_node, const pair<Key, Value>& pair)
	//{
	//    if (middle_node->full()) {
	//        this->create_new_root(middle_node, pair);
	//    } else {
	//        this->create_new_branch(middle_node, pair);
	//    }
	//}

	//BTREE_TEMPLATE
	//void
	//BTREE_INSTANCE::create_new_branch(const node_instance_type* node, const pair<Key, Value>& pair)
	//{
	//    auto up = node;
	//
	//    do {
	//        auto middle = new node_instance_type(this, middle_type());
	//        auto max = node->max_leaf();
	//
	//        up->middle_append({ pair.first, middle });
	//        max->next_node_ = middle;
	//
	//        up = middle;
	//        node = max;
	//    } while (!node->middle);
	//
	//    auto leaf = new node_instance_type(this, leaf_type(), &pair, &pair + 1);
	//    up->middle_append({ pair.first, leaf });
	//    node->next_node_ = leaf;
	//}
	//
	//BTREE_TEMPLATE
	//void
	//BTREE_INSTANCE::create_new_root(const node_instance_type* middle_node, const pair<Key, Value>& pair)
	//{
	//    auto p = make_pair<Key, unique_ptr<node_instance_type>>(root_->max_key(), root_.get());
	//    node_instance_type* new_root(new node_instance_type(this, middle_type(), &p, &p + 1));
	//
	//    this->create_new_branch(new_root, pair);
	//    root_ = nullptr;
	//    root_.reset(new_root);
	//}

	BTREE_TEMPLATE
		void
		BTREE_INSTANCE::merge_branch(const Key max_key, const node_instance_type* node)
	{
		array<pair<Key, unique_ptr<node_instance_type>>, 2> sons{
			{ root_->max_key(),root_ },
			{ max_key, node },
		};
		root_->next_node_ = node;
		auto new_root = new Node(this, middle_type(), sons.begin(), sons.end());

		root_.release();
		root_.reset(new_root);
	}

	/// if not exist, will add
	BTREE_TEMPLATE
		RESULT_FLAG
		BTREE_INSTANCE::modify(const pair<Key, Value>& pair)
	{
		auto& k = pair.first;
		auto& v = pair.second;

		node_instance_type*&& node = this->check_out(k);
		if (!node->middle) {
			// TODO should think let the if logic below work by Node-self
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
		BTREE_INSTANCE::explore() const
	{
		vector<Key> keys;
		keys.reserve(key_num_);
		this->traverse_leaf([&keys](node_instance_type* n)
		{
			for (auto&& k : n->all_key()) {
				keys.push_back(k);
			}
			return false;
		});

		return keys;
	}

	BTREE_TEMPLATE
		void
		BTREE_INSTANCE::remove(const Key& key)
	{
		node_instance_type*&& n = this->check_out(key);
		if (!n->have(key)) {
			return;
		}
		n->remove(key);
		--key_num_;
	}

	BTREE_TEMPLATE
		bool
		BTREE_INSTANCE::have(const Key& key) const
	{
		node_instance_type* r = this->check_out(key);
		return !(r->middle || !r->have(key));
	}

	// private method part:

	/// search the key in Btree, return the node that terminated, may be not have the key
	/// this is to save the search information
	BTREE_TEMPLATE
		typename BTREE_INSTANCE::node_instance_type*
		BTREE_INSTANCE::check_out(const Key& key) const
	{
		if (!root_->middle) {
			return root_.get();
		} else {
			return check_out_digging(key, root_.get());
		}
	}

	BTREE_TEMPLATE
		typename BTREE_INSTANCE::node_instance_type*
		BTREE_INSTANCE::check_out_digging(const Key& key, node_instance_type* node)
	{
		do {
			if (node->have(key)) {
				node = node->operator[](key);
			} else {
				return node;
			}
		} while (node->middle);

		return node;
	}

	/// operate on the true Node
	BTREE_TEMPLATE
		vector<typename BTREE_INSTANCE::node_instance_type*>
		BTREE_INSTANCE::traverse_leaf(const predicate& predicate) const
	{
		vector<node_instance_type*> result;

		node_instance_type* current = this->smallest_leaf();
		do {
			if (predicate(current)) {
				result.push_back(current);
			}
			// update
			current = current->next_node();
		} while (current != nullptr);

		return result;
	}

	BTREE_TEMPLATE
	typename BTREE_INSTANCE::node_instance_type*
	BTREE_INSTANCE::smallest_leaf() const
	{
		// TODO modify
		node_instance_type* current_node = root_.get();

		while (current_node->middle) {
			current_node = current_node->min_leaf();
		}

		return current_node;
	}

	BTREE_TEMPLATE
	typename BTREE_INSTANCE::node_instance_type*
	BTREE_INSTANCE::biggest_leaf()
	{
		// TODO modify
		node_instance_type* current_node = root_.get();

		while (current_node->middle) {
			current_node = current_node->max_leaf();
		}

		return current_node;
	}

	//BTREE_TEMPLATE
	//typename BTREE_INSTANCE::node_instance_type*
	//BTREE_INSTANCE::extreme_leaf(function<node_instance_type*()> node_method) {
	//    node_instance_type* current_node = root_.get();
	//
	//    while (current_node->middle) {
	//        current_node = current_node->node_method();
	//    }
	//
	//    return current_node;
	//}

}

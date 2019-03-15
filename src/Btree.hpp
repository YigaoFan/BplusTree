#pragma once

#include <vector> // for vector
#include <functional> // for lambda & less
#include <memory> // for shared_ptr, allocator
#include <utility> // for pair
#include "CommonFlag.hpp"
#include "BtreeHelper.hpp"
//#include "Node.hpp"
#include "NodeBase.hpp"
#include "LeafNode.hpp"
#include "MiddleNode.hpp"

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
//		using node_instance = Node<Key, Value, BtreeOrder, Btree>;
		using Node = NodeBase<Key, Value, BtreeOrder, Btree>;
		using Leaf = LeafNode<Key, Value, BtreeOrder, Btree>;
		using Middle = MiddleNode<Key, Value, BtreeOrder, Btree>;

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

		const Value* search(const Key&) const;
		RESULT_FLAG add(const std::pair<Key, Value>&);
		RESULT_FLAG modify(const std::pair<Key, Value>&);
		std::vector<Key> explore() const;
		void remove(const Key&);
		bool have(const Key&) const;

	private:
		unsigned key_num_ = 0;
		using predicate = std::function<bool(Leaf*)>;
		std::unique_ptr<Node> root_{nullptr};

		Node* check_out(const Key&) const;
		static Node* check_out_digging(const Key&, Middle*);
		std::vector<Leaf*> traverse_leaf(const predicate&) const;
		Leaf* smallest_leaf() const;
		Leaf* biggest_leaf() const;
		//node_instance* extreme_leaf(std::function<node_instance*()>);

		template <bool FirstFlag, typename Element, std::size_t NodeCount>
		void helper(const std::array<Element, NodeCount>&);

		//void root_add(const node_instance*, const std::pair<Key, Value>&);
		//void create_new_branch(const node_instance*, const std::pair<Key, Value>&);
		//void create_new_root(const node_instance*, const std::pair<Key, Value>&);
		void merge_branch(Key, const Node*); // for Node merge a branch
		// 暂定为 Node 类型，以及 Key 为传值还是引用未定
	};
}

#include <algorithm>  // for sort
#include <array> // for array
#include <exception> // for exception

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

#define BTREE_INSTANCE Btree<Key, Value, BtreeOrder>

	// all method in this level,
	// when call lower level function, must ensure the Key exist
	// public method part:

	/// recommend in constructor construct all Key-Value
	/// user should ensure not include the same Key-Value
	/// Because when construct a tree, you can see all Keys
	/// When you add or modify, you can use have() function to check it
	template <typename Key, typename Value, unsigned BtreeOrder>
	template <std::size_t NumOfArrayEle>
	Btree<Key, Value, BtreeOrder>::Btree(const compare& compare_function, array<pair<Key, Value>, NumOfArrayEle> pair_array)
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
			root_.reset(new Leaf(*this, pair_array.begin(), pair_array.end()));
		} else {
			this->helper<true>(pair_array);
		}

		key_num_ += NumOfArrayEle;
	}

	template <typename Key, typename Value, unsigned BtreeOrder>
	Btree<Key, Value, BtreeOrder>::Btree(const Btree& that)
		: BtreeHelper(that), key_num_(that.key_num_), root_(that.root_), compare_func_(that.compare_func_)
	{
		// copy constructor	TODO
	}

	template <typename Key, typename Value, unsigned BtreeOrder>
	Btree<Key, Value, BtreeOrder>::Btree(Btree&& that)
		: BtreeHelper(that), key_num_(that.key_num_), root_(that.root_), compare_func_(that.compare_func_)
	{
		// move constructor	TODO
	}

	template <typename Key, typename Value, unsigned BtreeOrder>
	Btree<Key, Value, BtreeOrder>&
	Btree<Key, Value, BtreeOrder>::operator=(const Btree& that)
	{
		// copy assign TODO
	}

	template <typename Key, typename Value, unsigned BtreeOrder>
	Btree<Key, Value, BtreeOrder>&
	Btree<Key, Value, BtreeOrder>::operator=(Btree&& that)
	{
		// move assign TODO
	}

	template <typename Key, typename Value, unsigned BtreeOrder>
	template <bool FirstCall, typename ElementType, std::size_t NodeCount>
	void
	Btree<Key, Value, BtreeOrder>::helper(const array<ElementType, NodeCount>& nodes)
	{
		constexpr auto upper_node_num = (NodeCount % BtreeOrder == 0) ? (NodeCount / BtreeOrder) : (NodeCount / BtreeOrder + 1);
		// store all nodes
		array<pair<Key, Node*>, upper_node_num> all_upper_node;

		auto head = nodes.begin();
		auto end = nodes.end();
		auto i = 0;
		auto tail = head + BtreeOrder;
		auto first = true;

		do {
			if constexpr (FirstCall) {
				auto leaf = new Leaf(*this, head, tail);

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
				auto middle = new Middle(*this, head, tail);

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
			root_.reset(new Middle(*this, all_upper_node.begin(), all_upper_node.end()));
		} else {
			// recursive
			this->helper<false>(all_upper_node);
		}
	}

	// TODO wait test
	template <typename Key, typename Value, unsigned BtreeOrder>
	template <unsigned long NumOfArrayEle>
	Btree<Key, Value, BtreeOrder>::Btree(compare&& compare_function, array<pair<Key, Value>, NumOfArrayEle>&& pair_array)
		: Btree(compare_function, pair_array) {}

	template <typename Key, typename Value, unsigned BtreeOrder>
	const Value*
	Btree<Key, Value, BtreeOrder>::search(const Key& key) const
	{
		Node* node = this->check_out(key);
		if (node->middle) {
			return nullptr;
		}
		return static_cast<Leaf*>(node)->operator[](key);
	}

	template <typename Key, typename Value, unsigned BtreeOrder>
	RESULT_FLAG
	Btree<Key, Value, BtreeOrder>::add(const pair<Key, Value>& pair)
	{
		if (root_ == nullptr) {
			root_.reset(new Leaf(*this, &pair, &pair + 1));
			++key_num_;
			return OK;
		}

		auto& k = pair.first;
		auto& v = pair.second;
		Node* node = this->check_out(k);
		// all the pair is inserted to Leaf below
		if (!node->middle) {
			if (node->have(k)) {
				throw runtime_error("The key-value has already existed, can't be added.");
			} else {
				static_cast<Leaf*>(node)->add(pair);
				++key_num_;
			}
		} else {
			auto leaf = this->biggest_leaf();
			leaf->add(pair);
			++key_num_;
		}
		return OK;
	}

	//template <typename Key, typename Value, unsigned BtreeOrder>
	//void
	//Btree<Key, Value, BtreeOrder>::root_add(const node_instance* middle_node, const pair<Key, Value>& pair)
	//{
	//    if (middle_node->full()) {
	//        this->create_new_root(middle_node, pair);
	//    } else {
	//        this->create_new_branch(middle_node, pair);
	//    }
	//}

	//template <typename Key, typename Value, unsigned BtreeOrder>
	//void
	//Btree<Key, Value, BtreeOrder>::create_new_branch(const node_instance* node, const pair<Key, Value>& pair)
	//{
	//    auto up = node;
	//
	//    do {
	//        auto middle = new node_instance(this, middle_type());
	//        auto max = node->max_leaf();
	//
	//        up->middle_append({ pair.first, middle });
	//        max->next_node_ = middle;
	//
	//        up = middle;
	//        node = max;
	//    } while (!node->middle);
	//
	//    auto leaf = new node_instance(this, leaf_type(), &pair, &pair + 1);
	//    up->middle_append({ pair.first, leaf });
	//    node->next_node_ = leaf;
	//}
	//
	//template <typename Key, typename Value, unsigned BtreeOrder>
	//void
	//Btree<Key, Value, BtreeOrder>::create_new_root(const node_instance* middle_node, const pair<Key, Value>& pair)
	//{
	//    auto p = make_pair<Key, unique_ptr<node_instance>>(root_->max_key(), root_.get());
	//    node_instance* new_root(new node_instance(this, middle_type(), &p, &p + 1));
	//
	//    this->create_new_branch(new_root, pair);
	//    root_ = nullptr;
	//    root_.reset(new_root);
	//}

	template <typename Key, typename Value, unsigned BtreeOrder>
	void
	Btree<Key, Value, BtreeOrder>::merge_branch(Key max_key, const node_instance* node)
	{
		array<pair<Key, unique_ptr<node_instance>>, 2> sons{
			{ root_->max_key(),root_ },
			{ max_key, node },
		};
		root_->next_node_ = node;
		auto new_root = new Node(this, middle_type(), sons.begin(), sons.end());

		root_.release();
		root_.reset(new_root);
	}

	/// if not exist, will add
	template <typename Key, typename Value, unsigned BtreeOrder>
	RESULT_FLAG
	Btree<Key, Value, BtreeOrder>::modify(const pair<Key, Value>& pair)
	{
		auto& k = pair.first;
		auto& v = pair.second;

		Node* node = this->check_out(k);
		if (!node->middle) {
			if (node->have(k)) {
				static_cast<Leaf*>(node)->operator[](k) = v;
				return OK;
			}
		}

		throw runtime_error("Precondition is not met, the corresponding key does not exist.");
	}

	template <typename Key, typename Value, unsigned BtreeOrder>
	vector<Key>
	Btree<Key, Value, BtreeOrder>::explore() const
	{
		vector<Key> keys;
		keys.reserve(key_num_);
		this->traverse_leaf([&keys](Node* n) {
			for (auto&& k : n->all_jjkey()) {
				keys.push_back(k);
			}
			return false;
		});

		return keys;
	}

	template <typename Key, typename Value, unsigned BtreeOrder>
	void
	Btree<Key, Value, BtreeOrder>::remove(const Key& key)
	{
		Node* n = this->check_out(key);
		if (!n->have(key)) {
			return;
		}
		n->remove(key); // TODO logic may be wrong here
		--key_num_;
	}

	template <typename Key, typename Value, unsigned BtreeOrder>
	bool
	Btree<Key, Value, BtreeOrder>::have(const Key& key) const
	{
		Node* r = this->check_out(key);
		return !(r->middle || !r->have(key));
	}

	// private method part:

	/// search the key in Btree, return the node that terminated, may be not have the key
	/// this is to save the search information
	template <typename Key, typename Value, unsigned BtreeOrder>
	typename Btree<Key, Value, BtreeOrder>::Node*
	Btree<Key, Value, BtreeOrder>::check_out(const Key& key) const
	{
		if (!root_->middle) {
			return root_.get();
		} else {
			return check_out_digging(key, root_.get());
		}
	}

	template <typename Key, typename Value, unsigned BtreeOrder>
	typename Btree<Key, Value, BtreeOrder>::Node*
	Btree<Key, Value, BtreeOrder>::check_out_digging(const Key& key, Middle* node)
	{
		// the call function check_out has ensure middle=true on the first time
		do {
			if (node->have(key)) {
				node = node->operator[](key);
			} else {
				return node;
			}
		} while (node->middle);

		return node; // node is Leaf here
	}

	/// operate on the true Node
	template <typename Key, typename Value, unsigned BtreeOrder>
	vector<typename Btree<Key, Value, BtreeOrder>::Leaf*>
	Btree<Key, Value, BtreeOrder>::traverse_leaf(const predicate& predicate) const
	{
		vector<Leaf*> result;

		Leaf* current = this->smallest_leaf();
		do {
			if (predicate(current)) {
				result.push_back(current);
			}
			// update
			current = current->next_node();
		} while (current != nullptr);

		return result;
	}

	template <typename Key, typename Value, unsigned BtreeOrder>
	typename Btree<Key, Value, BtreeOrder>::Leaf*
	Btree<Key, Value, BtreeOrder>::smallest_leaf() const
	{
		Node* current_node = root_.get();

		while (current_node->middle) {
			current_node = current_node->min_leaf();
		}

		return static_cast<Leaf*>(current_node);
	}

	template <typename Key, typename Value, unsigned BtreeOrder>
	typename Btree<Key, Value, BtreeOrder>::Leaf*
	Btree<Key, Value, BtreeOrder>::biggest_leaf() const
	{
		Node* current_node = root_.get();

		while (current_node->middle) {
			current_node = current_node->max_leaf();
		}

		return static_cast<Leaf*>(current_node);
	}

	//template <typename Key, typename Value, unsigned BtreeOrder>
	//typename Btree<Key, Value, BtreeOrder>::node_instance*
	//Btree<Key, Value, BtreeOrder>::extreme_leaf(function<node_instance*()> node_method) {
	//    node_instance* current_node = root_.get();
	//
	//    while (current_node->middle) {
	//        current_node = current_node->node_method();
	//    }
	//
	//    return current_node;
	//}

}

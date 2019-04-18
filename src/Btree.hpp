#pragma once
#include <vector> // for vector
#include <functional> // for lambda & less
#include <memory> // for shared_ptr, allocator
#include <utility> // for pair
#include "CommonFlag.hpp"
#include "BtreeHelper.hpp"
#include "NodeBase.hpp"
#include "LeafNode.hpp"
#include "MiddleNode.hpp"

#ifdef BTREE_DEBUG
#include <iostream>
#include "Utility.hpp"
#endif

// declaration
namespace btree {
	template <typename Key, typename Value, int16_t BtreeOrder>
	class Btree : private BtreeHelper {
	public:
		const std::function<bool(const Key, const Key)> compareFunc;

		template <std::size_t NumOfArrayEle>
		Btree(const std::function<bool(const Key, const Key)>&, std::array<std::pair<Key, Value>, NumOfArrayEle>);
		template <int16_t NumOfArrayEle>
		Btree(std::function<bool(const Key, const Key)>&&, std::array<std::pair<Key, Value>, NumOfArrayEle>&&);
		Btree(const Btree&);
		Btree(Btree&&) noexcept;
		~Btree() override = default;
		Btree& operator=(const Btree&);
		Btree& operator=(Btree&&) noexcept;

		const Value* search(const Key&) const;
		void add(const std::pair<Key, Value>&);
		void modify(const std::pair<Key, Value>&);
		std::vector<Key> explore() const;
		void remove(const Key&);
		bool have(const Key&) const;

	private:
		using Node = NodeBase<Key, Value, BtreeOrder, Btree>;
		using Leaf = LeafNode<Key, Value, BtreeOrder, Btree>;
		using Middle = MiddleNode<Key, Value, BtreeOrder, Btree>;

		int16_t _keyNum { 0 };
		std::unique_ptr<Node> _root { nullptr };

		Node* checkOut(const Key&) const;
		static Node* checkOutDigging(const Key&, Middle*);
		std::vector<Leaf*> traverseLeaf(const std::function<bool(Leaf*)>&) const;
		Node* CloneNodes(Btree&) const;
		template <bool FirstFlag, typename Element, std::size_t NodeCount>
		void constructNodeHierarchy(const std::array<Element, NodeCount>&);

		//void root_add(const node_instance*, const std::pair<Key, Value>&);
		//void create_new_branch(const node_instance*, const std::pair<Key, Value>&);
		//void create_new_root(const node_instance*, const std::pair<Key, Value>&);
//		void merge_branch(Key, const Node*); // for Node merge a branch
		// 暂定为 Node 类型，以及 Key 为传值还是引用未定
	};

	// Tool
	template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
	LeafNode<Key, Value, BtreeOrder, BtreeType>* maxLeaf(NodeBase<Key, Value, BtreeOrder, BtreeType>*);
	template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
	LeafNode<Key, Value, BtreeOrder, BtreeType>* minLeaf(NodeBase<Key, Value, BtreeOrder, BtreeType>*);
}

#include <algorithm>  // for sort
#include <array> // for array
#include <exception> // for exception

#ifdef BTREE_DEBUG
#include <cassert>
#endif
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

// implementation
namespace btree {
	// public method part:
	/**
	 * @brief Constructor. Recommend in constructor construct all Key-Value in this method,
	 * user should ensure not include the duplicate Key pairs.
	 * When you add or modify a pair, if you're not sure the existence of it,
	 * you could use have() function to check it
	 * @tparam Key
	 * @tparam Value
	 * @tparam BtreeOrder
	 * @param compareFunction two Key type in, bool out
	 * @param pairArray An array contained pairs of key and value
	 */
	template <typename Key, typename Value, int16_t BtreeOrder>
	template <std::size_t ElementNumber>
	Btree<Key, Value, BtreeOrder>::Btree(
		const std::function<bool(const Key, const Key)>& compareFunction,
		array<pair<Key, Value>, ElementNumber> pairArray
	)
		: BtreeHelper(), compareFunc(compareFunction)
	{
		if constexpr (ElementNumber == 0) { return; }

		// sort
		sort(pairArray.begin(),
			 pairArray.end(),
			 [&](const pair<Key, Value>& p1, const pair<Key, Value>& p2) {
				 // duplicate check
				 if (compareFunction(p1.first, p2.first) && compareFunction(p2.first, p1.first)) {
					 throw runtime_error("The input array has the duplicate key");
				 } else {
					 return compareFunc(p1.first, p2.first);
				 }});

		// construct from bottom
		if constexpr (ElementNumber <= BtreeOrder) {
			_root.reset(new Leaf(*this, pairArray.begin(), pairArray.end()));
		} else {
			this->constructNodeHierarchy<true>(pairArray);
		}

		_keyNum += ElementNumber;
	}

	template <typename Key, typename Value, int16_t BtreeOrder>
	template <bool FirstCall, typename ElementType, std::size_t NodeCount>
	void
	Btree<Key, Value, BtreeOrder>::constructNodeHierarchy(const array<ElementType, NodeCount>& nodesMaterial)
	{
		constexpr auto upperNodeNum = (NodeCount % BtreeOrder == 0) ? (NodeCount / BtreeOrder) : (NodeCount / BtreeOrder + 1);
		array<Node*, upperNodeNum> upperNodes;

		auto head = nodesMaterial.begin();
		auto end = nodesMaterial.end();
		auto i = 0;
		auto tail = head + BtreeOrder;
		auto first = true;

		do {
			if constexpr (FirstCall) {
				auto leaf = new Leaf(*this, head, tail);

#ifdef BTREE_DEBUG
				// defense code
				assert(i < upperNodeNum);
#endif

				upperNodes[i] = leaf;
				if (first) {
					first = false;
				} else {
					// set nextLeaf of Node
					static_cast<Leaf*>(upperNodes[i - 1])->next_leaf(leaf);
				}
			} else {
				auto middle = new Middle(*this, head, tail);

#ifdef BTREE_DEBUG
				//defense code
				assert(i < upperNodeNum);
#endif

				upperNodes[i] = middle;
				throw runtime_error("Got here"); // TODO Occur problem
			}

			// update
			head = tail;
			tail = (end - tail > BtreeOrder) ? (end + BtreeOrder) : end;
			++i;
		} while (end - head > 0);

		if constexpr (upperNodeNum <= BtreeOrder) {
			_root.reset(new Middle(*this, upperNodes.begin(), upperNodes.end()));
		} else {
			// recursive
			this->constructNodeHierarchy<false>(upperNodes);
		}
	}


	template <typename Key, typename Value, int16_t BtreeOrder>
	Btree<Key, Value, BtreeOrder>::Btree(const Btree& that)
		: BtreeHelper(that), _keyNum(that._keyNum), _root(that.CloneNodes(*this)), compareFunc(that.compareFunc)
	{
		// copy constructor
	}

	template <typename Key, typename Value, int16_t BtreeOrder>
	Btree<Key, Value, BtreeOrder>::Btree(Btree&& that) noexcept
		: BtreeHelper(that), _keyNum(that._keyNum), _root(that._root.release()), compareFunc(that.compareFunc)
	{
		// move constructor
	}

	template <typename Key, typename Value, int16_t BtreeOrder>
	Btree<Key, Value, BtreeOrder>&
	Btree<Key, Value, BtreeOrder>::operator=(const Btree& that)
	{
		// copy assign
		static_cast<BtreeHelper>(*this) = that;
		this->_keyNum = that._keyNum;
		this->_root.reset(that.CloneNodes(*this));
		this->compareFunc = that.compareFunc;
	}

	template <typename Key, typename Value, int16_t BtreeOrder>
	Btree<Key, Value, BtreeOrder>&
	Btree<Key, Value, BtreeOrder>::operator=(Btree&& that) noexcept
	{
		// move assign
		static_cast<BtreeHelper>(*_root) = that;
		this->_keyNum = that._keyNum;
		this->_root.reset(that._root.release());
		this->compareFunc = that.compareFunc;
	}


	// TODO wait test
	template <typename Key, typename Value, int16_t BtreeOrder>
	template <int16_t NumOfArrayEle>
	Btree<Key, Value, BtreeOrder>::Btree(std::function<bool(const Key, const Key)>&& compare_function, array<pair<Key, Value>, NumOfArrayEle>&& pair_array)
		: Btree(compare_function, pair_array) {}

	template <typename Key, typename Value, int16_t BtreeOrder>
	const Value*
	Btree<Key, Value, BtreeOrder>::search(const Key& key) const
	{
		Node* node = this->checkOut(key);
		if (node->Middle) {
			return nullptr;
		}
		return node->self()->operator[](key);
	}

	template <typename Key, typename Value, int16_t BtreeOrder>
	void
	Btree<Key, Value, BtreeOrder>::add(const pair<Key, Value>& pair)
	{
		if (_root == nullptr) {
			_root.reset(new Leaf(*this, &pair, &pair + 1));
			++_keyNum;
			return;
		}

		auto& k = pair.first;
		auto& v = pair.second;
		Node* node = this->checkOut(k);
		// all the pair is inserted to Leaf below
		if (!node->Middle) {
			if (node->have(k)) {
				throw runtime_error("The key-value has already existed, can't be added.");
			} else {
				node->self()->add(pair);
				++_keyNum;
			}
		} else {
			auto leaf = maxLeaf(_root.get());
			leaf->add(pair);
			++_keyNum;
		}
	}



	template <typename Key, typename Value, int16_t BtreeOrder>
	void
	Btree<Key, Value, BtreeOrder>::modify(const pair<Key, Value>& pair)
	{
		auto& k = pair.first;
		auto& v = pair.second;

		Node* node = this->checkOut(k);
		if (!node->Middle) {
			if (node->have(k)) {
				node->self()->operator[](k) = v;
				return;
			}
		}

		throw runtime_error("Precondition is not met, the corresponding key does not exist.");
	}

	template <typename Key, typename Value, int16_t BtreeOrder>
	vector<Key>
	Btree<Key, Value, BtreeOrder>::explore() const
	{
		vector<Key> keys;
		keys.reserve(_keyNum);
		this->traverseLeaf([&keys](Node* n) {
			for (auto&& k : n->all_key()) {
				keys.push_back(k);
			}
			return false;
		});

		return keys;
	}

	template <typename Key, typename Value, int16_t BtreeOrder>
	void
	Btree<Key, Value, BtreeOrder>::remove(const Key& key)
	{
		Node* n = this->checkOut(key);
		if (!n->have(key)) {
			return;
		}
		n->remove(key); // TODO logic may be wrong here
		--_keyNum;
	}

	template <typename Key, typename Value, int16_t BtreeOrder>
	bool
	Btree<Key, Value, BtreeOrder>::have(const Key& key) const
	{
		Node* r = this->checkOut(key);
		return !(r->Middle || !r->have(key));
	}

	// private method part:

	/// search the key in Btree, return the node that terminated, may be not have the key
	template <typename Key, typename Value, int16_t BtreeOrder>
	typename Btree<Key, Value, BtreeOrder>::Node*
	Btree<Key, Value, BtreeOrder>::checkOut(const Key& key) const
	{
		if (!_root->Middle) {
			return _root.get();
		} else {
			return checkOutDigging(key, _root.get()->self());
		}
	}

	template <typename Key, typename Value, int16_t BtreeOrder>
	typename Btree<Key, Value, BtreeOrder>::Node*
	Btree<Key, Value, BtreeOrder>::checkOutDigging(const Key& key, Middle* node)
	{
		// call function checkOut has ensure Middle=true on the first time
		do {
			if (node->have(key)) {
				node = node->operator[](key);
			} else {
				return node;
			}
		} while (node->Middle);

		return node; // node is Leaf here
	}

	/// operate on the true Node
	template <typename Key, typename Value, int16_t BtreeOrder>
	vector<typename Btree<Key, Value, BtreeOrder>::Leaf*>
	Btree<Key, Value, BtreeOrder>::traverseLeaf(const std::function<bool(Leaf*)>& predicate) const
	{
		vector<Leaf*> result;

		Leaf* current = minLeaf(_root.get());
		do {
			if (predicate(current)) {
				result.push_back(current);
			}
			// update
			current = current->nextLeaf();
		} while (current != nullptr);

		return result;
	}

	template <typename Key, typename Value, int16_t BtreeOrder>
	typename Btree<Key, Value, BtreeOrder>::Node*
	Btree<Key, Value, BtreeOrder>::CloneNodes(Btree& newBtree) const
	{
		if (!_root->Middle) {
			auto l = new Leaf(*(_root.get()->self()), newBtree);
			return l;
		}

		vector<Leaf*> old_leafs= this->traverseLeaf([](Leaf*) -> bool {
			return true;
		});

		vector<Node*> oldChilds{};
		oldChilds.reserve(_keyNum);
		for (auto& l: old_leafs) {
			oldChilds.emplace_back(l);
		}

        vector<Node*> newChilds{};
        newChilds.reserve(_keyNum);
        Leaf* next = nullptr;
        for (auto b = oldChilds.rbegin(), e = oldChilds.rend(); b != e; ++b) {
            auto l = new Leaf(*b, newBtree, next);
            newChilds.emplace_back(l);
            next = l;
        }

		do {
			auto i = 0;
			vector<Node*> new_fathers{};
			vector<Node*> old_fathers{};

			do {
#ifdef BTREE_DEBUG
				assert(oldChilds[i]->father() == nullptr);
#endif
				auto child_num = oldChilds[i]->father()->child_count();
				old_fathers.emplace(oldChilds[i]->father());

				new_fathers.emplace(new Middle(
					newBtree,
					newChilds.begin() + i,
					newChilds.begin() + i + child_num
				));
				// update
				i += child_num;
			} while (i < oldChilds.size());

			// update
			std::swap(newChilds, new_fathers);
			std::swap(oldChilds, old_fathers);
		} while (newChilds.size() > 1);

		return newChilds[0];

		// construct leaf from old leaf
		// construct Middle from leaf
		// construct upper-Middle from Middle, need some info from old leaf

		// so old need to be reserved, but need to be updated to upper level on time
		// relatived-child vector
		// relatived-father vector
	}

	template<typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
	LeafNode<Key, Value, BtreeOrder, BtreeType>*
	minLeaf(NodeBase<Key, Value, BtreeOrder, BtreeType>* node)
	{
		std::function<NodeBase<Key, Value, BtreeOrder, BtreeType>*(MiddleNode<Key, Value, BtreeOrder, BtreeType>*)> min
			= [] (auto n) {
				return n->min_son();
			}; // in normal situation, the auto should use on the left side I think, here is for explicit type
		return node_select_recur(node, min);
	}

	template<typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
	LeafNode<Key, Value, BtreeOrder, BtreeType>*
	maxLeaf(NodeBase<Key, Value, BtreeOrder, BtreeType>* node)
	{
		std::function<NodeBase<Key, Value, BtreeOrder, BtreeType>*(MiddleNode<Key, Value, BtreeOrder, BtreeType>*)> max
			= [] (auto n) {
				return n->max_son();
			};
		return node_select_recur(node, max);
	}


	template <typename Key, typename Value, int16_t BtreeOrder, typename BtreeType>
	LeafNode<Key, Value, BtreeOrder, BtreeType>*
	node_select_recur(
		NodeBase<Key, Value, BtreeOrder, BtreeType>* node,
		const std::function<NodeBase<Key, Value, BtreeOrder, BtreeType>* (MiddleNode<Key, Value, BtreeOrder, BtreeType>*)>& operation
	)
	{
		while (node->Middle) {
			node = operation(node->self());
		}

		return node->self();
	}
}
//template <typename Key, typename Value, int16_t BtreeOrder>
	//void
	//Btree<Key, Value, BtreeOrder>::root_add(const node_instance* middle_node, const pair<Key, Value>& pair)
	//{
	//    if (middle_node->full()) {
	//        this->create_new_root(middle_node, pair);
	//    } else {
	//        this->create_new_branch(middle_node, pair);
	//    }
	//}

	//template <typename Key, typename Value, int16_t BtreeOrder>
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
	//    } while (!node->Middle);
	//
	//    auto leaf = new node_instance(this, leaf_type(), &pair, &pair + 1);
	//    up->middle_append({ pair.first, leaf });
	//    node->next_node_ = leaf;
	//}
	//
	//template <typename Key, typename Value, int16_t BtreeOrder>
	//void
	//Btree<Key, Value, BtreeOrder>::create_new_root(const node_instance* middle_node, const pair<Key, Value>& pair)
	//{
	//    auto p = make_pair<Key, unique_ptr<node_instance>>(_root->maxKey(), _root.get());
	//    node_instance* new_root(new node_instance(this, middle_type(), &p, &p + 1));
	//
	//    this->create_new_branch(new_root, pair);
	//    _root = nullptr;
	//    _root.reset(new_root);
	//}

//	template <typename Key, typename Value, int16_t BtreeOrder>
//	void
//	Btree<Key, Value, BtreeOrder>::merge_branch(Key maxKey, const node_instance* node)
//	{
//		array<pair<Key, unique_ptr<node_instance>>, 2> sons{
//			{ _root->maxKey(),_root },
//			{ maxKey, node },
//		};
//		_root->next_node_ = node; // need to be changed
//		auto new_root = new Node(this, middle_type(), sons.begin(), sons.end());
//
//		_root.release();
//		_root.reset(new_root);
//	}

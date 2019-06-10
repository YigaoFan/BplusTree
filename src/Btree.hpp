#pragma once
#include <vector> 		// for vector
#include <functional> 	// for function
#include <memory> 		// for unique_ptr
#include <utility> 		// for pair
#include <algorithm>  	// for sort
#include <array> 		// for array
#include <exception> 	// for exception
#include "LeafNode.hpp"
#include "MiddleNode.hpp"

#ifdef BTREE_DEBUG
#include <iostream>
#include "Utility.hpp"
#include <cassert>
#endif

// declaration
namespace btree {
	using std::function;
	using std::array;
	using std::pair;
	using std::make_pair;
	using std::vector;
	using std::sort;
	using std::exception;
	using std::runtime_error;
	using std::unique_ptr;
	using std::make_shared;
	using std::size_t;

#define BTREE_TEMPLATE template <uint16_t BtreeOrder, typename Key, typename Value>

	BTREE_TEMPLATE
	class Btree {
		using Base   = NodeBase<Key, Value, BtreeOrder>;
		using Leaf   = LeafNode<Key, Value, BtreeOrder>;
		using Middle = MiddleNode<Key, Value, BtreeOrder>;
	public:
		using LessThan = typename Base::LessThan;
		// should in constructor have a arg for "if save some space for future insert"?
		template <size_t NumOfArrayEle>
		Btree(LessThan, array<pair<Key, Value>, NumOfArrayEle>);
		template <size_t NumOfArrayEle>
		Btree(LessThan, array<pair<Key, Value>, NumOfArrayEle>&&);
		Btree(const Btree&);
		Btree(Btree&&) noexcept;
		~Btree() = default;
		Btree& operator=(const Btree&);
		Btree& operator=(Btree&&) noexcept;

		Value       search(const Key&) const;
		void        add(pair<Key, Value>);
		void        modify(pair<Key, Value>);
		vector<Key> explore() const;
		void        remove(const Key&);
		bool        have(const Key&) const;

	private:
		shared_ptr<LessThan> lessThanPtr;
		uint16_t _keyNum { 0 };
		unique_ptr<Base> _root { nullptr };

		Base* checkOut(const Key&) const;
		static Base* checkOutHelper(const Key&, Middle*);
		vector<Leaf*> traverseLeaf(const std::function<bool(Leaf*)>&) const;
		Base* CloneNodes(Btree&) const;
		template <bool FirstCall = true, typename E, size_t NodeCount>
		void constructTreeFromBottomToRoot(const array<E, NodeCount>&);

		//void root_add(const node_instance*, const std::pair<Key, Value>&);
		//void create_new_branch(const node_instance*, const std::pair<Key, Value>&);
		//void create_new_root(const node_instance*, const std::pair<Key, Value>&);
//		void merge_branch(Key, const Base*); // for Base merge a branch
	};

	// Tool
	template <typename Key, typename Value, int16_t BtreeOrder>
	LeafNode<Key, Value, BtreeOrder>* maxLeaf(NodeBase<Key, Value, BtreeOrder>*);
	template <typename Key, typename Value, int16_t BtreeOrder>
	LeafNode<Key, Value, BtreeOrder>* minLeaf(NodeBase<Key, Value, BtreeOrder>*);
}

// implementation
namespace btree {
#define BTREE Btree<BtreeOrder, Key, Value>

	BTREE_TEMPLATE
	template <size_t NumOfArrayEle>
	BTREE::Btree(
		LessThan lessThan,
		array<pair<Key, Value>, NumOfArrayEle> pairArray
	) :
	lessThanPtr(make_shared<LessThan>(lessThan))
	{
		if constexpr (NumOfArrayEle == 0) { return; }

		sort(pairArray.begin(),
			 pairArray.end(),
			 [&] (auto& p1, auto& p2) {
				 return lessThan(p1.first, p2.first);
			 });

		if constexpr (NumOfArrayEle <= BtreeOrder) {
			_root.reset(new Leaf(pairArray.begin(), pairArray.end(), lessThanPtr));
		} else {
			constructTreeFromBottomToRoot(pairArray);
		}

		_keyNum += NumOfArrayEle;
	}

	BTREE_TEMPLATE
	template <bool FirstCall, typename E, size_t NodeCount>
	void
	BTREE::constructTreeFromBottomToRoot(const array<E, NodeCount>& nodesMaterial)
	{
		constexpr auto upperNodeNum = (NodeCount % BtreeOrder == 0) ? (NodeCount / BtreeOrder) : (NodeCount / BtreeOrder + 1);
		array<pair<Key, Base*>, upperNodeNum> upperNodes;

		auto head = nodesMaterial.begin();
		auto end = nodesMaterial.end();
		auto i = 0;
		auto tail = head + BtreeOrder;
		auto firstLeaf = true;

		do {
			if constexpr (FirstCall) {
				auto leaf = new Leaf(head, tail, lessThanPtr);
				upperNodes[i] = make_pair(leaf->maxKey(), leaf);
				if (firstLeaf) {
					firstLeaf = false;
				} else {
					static_cast<Leaf*>(upperNodes[i - 1].second)->nextLeaf(leaf);
				}
			} else {
				auto middle = new Middle(head, tail, lessThanPtr);
				upperNodes[i] = make_pair(middle->maxKey(), middle);
			}

			head = tail;
			tail = (end - tail > BtreeOrder) ? (tail + BtreeOrder) : end;
			++i;
		} while (end - head > 0);

		if constexpr (upperNodeNum <= BtreeOrder) {
			_root.reset(new Middle(upperNodes.begin(), upperNodes.end(), lessThanPtr));
		} else {
			constructTreeFromBottomToRoot<false>(upperNodes);
		}
	}


	BTREE_TEMPLATE
	BTREE::Btree(const Btree& that)
		: _keyNum(that._keyNum), _root(that.CloneNodes(*this)), lessThanPtr(that.lessThanPtr)
	{}

	BTREE_TEMPLATE
	BTREE::Btree(Btree&& that) noexcept
		: _keyNum(that._keyNum), _root(that._root.release()), lessThanPtr(that.lessThanPtr)
	{}

	BTREE_TEMPLATE
	BTREE&
	BTREE::operator=(const Btree& that)
	{
		this->_keyNum = that._keyNum;
		this->_root.reset(that.CloneNodes(*this));
		this->lessThanPtr = that.lessThanPtr;
	}

	BTREE_TEMPLATE
	BTREE&
	BTREE::operator=(Btree&& that) noexcept
	{
		// move assign
//		static_cast<BtreeHelper>(*_root) = that;
		this->_keyNum = that._keyNum;
		this->_root.reset(that._root.release());
		this->lessThanPtr = that.lessThanPtr;
	}

	BTREE_TEMPLATE
	Value
	BTREE::search(const Key& key) const
	{
		auto valuePtr = _root->search(key);
		if (valuePtr == nullptr) {
			throw runtime_error("The key you searched is beyond the max key.");
		}

		return *valuePtr;
	}

	BTREE_TEMPLATE
	void
	BTREE::add(pair<Key, Value> pair)
	{
		if (_root == nullptr) {
			_root.reset(new Leaf(&pair, &pair + 1, lessThanPtr));
		} else {
			if (_root->have(pair.first)) {
				throw runtime_error("The key-value has already existed, can't be added.");
			} else {
				// TODO how to reduce the duplicate search process
				_root->add(std::move(pair));
			}
		}

		++_keyNum;
	}

	BTREE_TEMPLATE
	void
	BTREE::modify(pair<Key, Value> pair)
	{
		if (_root == nullptr) {
			throw runtime_error(
				"Unable to modify, because the tree has no key-values");
		}

		auto& k = pair.first;
		auto& v = pair.second;

		if (_root->have(k)) {
			*(_root->search(k)) = v;
		}

		throw runtime_error(
			"Unable to modify, because the tree doesn't have the key of pair");
	}

	BTREE_TEMPLATE
	vector<Key>
	BTREE::explore() const
	{
		vector<Key> keys;
		keys.reserve(_keyNum);
		traverseLeaf([&keys](Base* n) {
			for (auto&& k : n->allKey()) {
				keys.push_back(k);
			}
			return false;
		});

		return keys;
	}

	BTREE_TEMPLATE
	void
	BTREE::remove(const Key& key)
	{
		Base* n = this->checkOut(key);
		if (!n->have(key)) {
			return;
		}
		n->remove(key); // TODO logic may be wrong here
		--_keyNum;
	}

	BTREE_TEMPLATE
	bool
	BTREE::have(const Key& key) const
	{
		return _root->have(key);
	}

	// private method part:

	/// search the key in Btree, return the node that terminated, may be not have the key
	BTREE_TEMPLATE
	typename BTREE::Base*
	BTREE::checkOut(const Key& key) const
	{
		if (!_root->Middle) {
			return _root.get();
		} else {
			return checkOutHelper(key, _root.get());
		}
	}

	BTREE_TEMPLATE
	typename BTREE::Base*
	BTREE::checkOutHelper(const Key& key, Middle* node)
	{
		do {
			if (node->have(key)) {
				node = node->operator[](key);
			} else {
				return node;
			}
		} while (node->Middle);

		return node; // node is Leaf here
	}

	BTREE_TEMPLATE
	vector<typename BTREE::Leaf*>
	BTREE::traverseLeaf(const function<bool(Leaf*)>& predicate) const
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

	BTREE_TEMPLATE
	typename BTREE::Base*
	BTREE::CloneNodes(Btree& newBtree) const
	{
		if (!_root->Middle) {
			auto l = new Leaf(*(_root.get()->self()), newBtree);
			return l;
		}

		vector<Leaf*> old_leafs= this->traverseLeaf([](Leaf*) -> bool {
			return true;
		});

		vector<Base*> oldChilds{};
		oldChilds.reserve(_keyNum);
		for (auto& l: old_leafs) {
			oldChilds.emplace_back(l);
		}

        vector<Base*> newChilds{};
        newChilds.reserve(_keyNum);
        Leaf* next = nullptr;
        for (auto b = oldChilds.rbegin(), e = oldChilds.rend(); b != e; ++b) {
            auto l = new Leaf(*b, newBtree, next);
            newChilds.emplace_back(l);
            next = l;
        }

		do {
			auto i = 0;
			vector<Base*> new_fathers{};
			vector<Base*> old_fathers{};

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

	template<typename Key, typename Value, int16_t BtreeOrder>
	typename BTREE::Leaf*
	minLeaf(NodeBase<Key, Value, BtreeOrder>* node)
	{
		std::function<NodeBase<Key, Value, BtreeOrder>*(MiddleNode<Key, Value, BtreeOrder>*)> min
			= [] (auto n) {
				return n->min_son();
			}; // in normal situation, the auto should use on the left side I think, here is for explicit type
		return node_select_recur(node, min);
	}

	template<typename Key, typename Value, int16_t BtreeOrder>
	LeafNode<Key, Value, BtreeOrder>*
	maxLeaf(NodeBase<Key, Value, BtreeOrder>* node)
	{
		function<NodeBase<Key, Value, BtreeOrder>*(MiddleNode<Key, Value, BtreeOrder>*)> max
			= [] (auto n) {
				return n->max_son();
			};
		return node_select_recur(node, max);
	}


	template <typename Key, typename Value, int16_t BtreeOrder>
	LeafNode<Key, Value, BtreeOrder>*
	node_select_recur(
		NodeBase<Key, Value, BtreeOrder>* node,
		const function<NodeBase<Key, Value, BtreeOrder>* (MiddleNode<Key, Value, BtreeOrder>*)>& operation
	)
	{
		while (node->Middle) {
			node = operation(node->self());
		}

		return node->self();
	}

#undef BTREE
#undef BTREE_TEMPLATE
}
//BTREE_TEMPLATE
	//void
	//BTREE::root_add(const node_instance* middle_node, const pair<Key, Value>& pair)
	//{
	//    if (middle_node->full()) {
	//        this->create_new_root(middle_node, pair);
	//    } else {
	//        this->create_new_branch(middle_node, pair);
	//    }
	//}

	//BTREE_TEMPLATE
	//void
	//BTREE::create_new_branch(const node_instance* node, const pair<Key, Value>& pair)
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
	//BTREE_TEMPLATE
	//void
	//BTREE::create_new_root(const node_instance* middle_node, const pair<Key, Value>& pair)
	//{
	//    auto p = make_pair<Key, unique_ptr<node_instance>>(_root->maxKey(), _root.get());
	//    node_instance* new_root(new node_instance(this, middle_type(), &p, &p + 1));
	//
	//    this->create_new_branch(new_root, pair);
	//    _root = nullptr;
	//    _root.reset(new_root);
	//}

//	BTREE_TEMPLATE
//	void
//	BTREE::merge_branch(Key maxKey, const node_instance* node)
//	{
//		array<pair<Key, unique_ptr<node_instance>>, 2> sons{
//			{ _root->maxKey(),_root },
//			{ maxKey, node },
//		};
//		_root->next_node_ = node; // need to be changed
//		auto new_root = new Base(this, middle_type(), sons.begin(), sons.end());
//
//		_root.release();
//		_root.reset(new_root);
//	}

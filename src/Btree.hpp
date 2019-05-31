#pragma once
#include <vector> 		// for vector
#include <functional> 	// for function
#include <memory> 		// for unique_ptr
#include <utility> 		// for pair
#include <algorithm>  	// for sort
#include <array> 		// for array
#include <exception> 	// for exception
//#include "BtreeHelper.hpp"
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
		static Base* checkOutDigging(const Key&, Middle*);
		vector<Leaf*> traverseLeaf(const std::function<bool(Leaf*)>&) const;
		Base* CloneNodes(Btree&) const;
		template <bool FirstFlag, typename Element, size_t NodeCount>
		void constructNodeHierarchy(const std::array<Element, NodeCount>&);

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

		// sort
		sort(pairArray.begin(),
			 pairArray.end(),
			 [&](const pair<Key, Value>& p1, const pair<Key, Value>& p2) {
				 if (!lessThan(p1.first, p2.first) && !lessThan(p2.first, p1.first)) {
					 throw runtime_error("Input array has the duplicate key");
				 } else {
					 return lessThan(p1.first, p2.first);
				 }
			 });

		// construct from bottom
		if constexpr (NumOfArrayEle <= BtreeOrder) {
			_root.reset(new Leaf(pairArray.begin(), pairArray.end(), make_shared(lessThan)));
		} else {
			this->constructNodeHierarchy<true>(pairArray);
		}

		_keyNum += NumOfArrayEle;
	}

	BTREE_TEMPLATE
	template <bool FirstCall, typename ElementType, size_t NodeCount>
	void
	BTREE::constructNodeHierarchy(const array<ElementType, NodeCount>& nodesMaterial)
	{
		constexpr auto upperNodeNum = (NodeCount % BtreeOrder == 0) ? (NodeCount / BtreeOrder) : (NodeCount / BtreeOrder + 1);
		array<Base*, upperNodeNum> upperNodes;

		auto head = nodesMaterial.begin();
		auto end = nodesMaterial.end();
		auto i = 0;
		auto tail = head + BtreeOrder;
		auto first = true;

		do {
			if constexpr (FirstCall) {
				auto leaf = new Leaf(head, tail, lessThanPtr);

				upperNodes[i] = leaf;
				if (first) {
					first = false;
				} else {
					// set nextLeaf of Base
					static_cast<Leaf*>(upperNodes[i - 1])->nextLeaf(leaf);
				}
			} else {
				auto middle = new Middle(head, tail, lessThanPtr);

				upperNodes[i] = middle;
				throw runtime_error("Got here"); // TODO Occur problem
			}

			// update
			head = tail;
			tail = (end - tail > BtreeOrder) ? (end + BtreeOrder) : end;
			++i;
		} while (end - head > 0);

		if constexpr (upperNodeNum <= BtreeOrder) {
			_root.reset(new Middle(upperNodes.begin(), upperNodes.end(), lessThanPtr));
		} else {
			// recursive
			this->constructNodeHierarchy<false>(upperNodes);
		}
	}


	BTREE_TEMPLATE
	BTREE::Btree(const Btree& that)
		: _keyNum(that._keyNum), _root(that.CloneNodes(*this)), lessThanPtr(that.lessThanPtr)
	{
		// copy constructor
	}

	BTREE_TEMPLATE
	BTREE::Btree(Btree&& that) noexcept
		: _keyNum(that._keyNum), _root(that._root.release()), lessThanPtr(that.lessThanPtr)
	{}

	BTREE_TEMPLATE
	BTREE&
	BTREE::operator=(const Btree& that)
	{
		// copy assign
//		static_cast<BtreeHelper>(*this) = that;
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
		Base* node = this->checkOut(key);
		if (node->Middle) {
//			throw exception;
		}
		return static_cast<Leaf>(*node)[key];
	}

	BTREE_TEMPLATE
	void
	BTREE::add(pair<Key, Value> pair)
	{
		if (_root == nullptr) {
			_root.reset(new Leaf(*this, &pair, &pair + 1));
			++_keyNum;
			return;
		}

		auto& k = pair.first;
		auto& v = pair.second;
		Base* node = this->checkOut(k);
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



	BTREE_TEMPLATE
	void
	BTREE::modify(pair<Key, Value> pair)
	{
		auto& k = pair.first;
		auto& v = pair.second;

		Base* node = this->checkOut(k);
		if (!node->Middle) {
			if (node->have(k)) {
				node->self()->operator[](k) = v;
				return;
			}
		}

		throw runtime_error("The key you want to modify does not exist.");
	}

	BTREE_TEMPLATE
	vector<Key>
	BTREE::explore() const
	{
		vector<Key> keys;
		keys.reserve(_keyNum);
		this->traverseLeaf([&keys](Base* n) {
			for (auto&& k : n->all_key()) {
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
		Base* r = this->checkOut(key);
		return !(r->Middle || !r->have(key));
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
			return checkOutDigging(key, _root.get()->self());
		}
	}

	BTREE_TEMPLATE
	typename BTREE::Base*
	BTREE::checkOutDigging(const Key& key, Middle* node)
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
	BTREE_TEMPLATE
	vector<typename BTREE::Leaf*>
	BTREE::traverseLeaf(const std::function<bool(Leaf*)>& predicate) const
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
		std::function<NodeBase<Key, Value, BtreeOrder>*(MiddleNode<Key, Value, BtreeOrder>*)> max
			= [] (auto n) {
				return n->max_son();
			};
		return node_select_recur(node, max);
	}


	template <typename Key, typename Value, int16_t BtreeOrder>
	LeafNode<Key, Value, BtreeOrder>*
	node_select_recur(
		NodeBase<Key, Value, BtreeOrder>* node,
		const std::function<NodeBase<Key, Value, BtreeOrder>* (MiddleNode<Key, Value, BtreeOrder>*)>& operation
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

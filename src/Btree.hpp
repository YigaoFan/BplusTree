#pragma once
#include <vector>       // for vector
#include <functional>   // for function
#include <memory>       // for unique_ptr
#include <utility>      // for pair
#include <algorithm>    // for sort
#include <array>        // for array
#include <exception>    // for exception
#include "LeafNode.hpp"
#include "MiddleNode.hpp"

#ifdef BTREE_DEBUG
#include <iostream>
#include "Utility.hpp"
#include <cassert>
#endif

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
		using Base   = NodeBase  <Key, Value, BtreeOrder>;
		using Leaf   = LeafNode  <Key, Value, BtreeOrder>;
		using Middle = MiddleNode<Key, Value, BtreeOrder>;

	public:
		using LessThan = typename Base::LessThan;
		// TODO should in constructor have a arg for "if save some space for future insert"?
		template <size_t NumOfArrayEle>
		Btree(LessThan, array<pair<Key, Value>, NumOfArrayEle>);
		template <size_t NumOfArrayEle>
		Btree(LessThan, array<pair<Key, Value>, NumOfArrayEle>&&);
		Btree(const Btree&);
		Btree(Btree&&) noexcept;
		Btree& operator=(const Btree&);
		Btree& operator=(Btree&&) noexcept;
        ~Btree() = default;

		Value       search (const Key&) const;
		void        add    (pair<Key, Value>);
		void        modify (pair<Key, Value>);
		vector<Key> explore() const;
		void        remove (const Key&);
		bool        have   (const Key&) const;
		bool        have   () const;

	private:
		shared_ptr<LessThan> _lessThanPtr;
		uint16_t             _keyNum { 0 };
		unique_ptr<Base>     _root { nullptr };

		vector<Leaf*> traverseLeaf(function<bool(Leaf*)>&) const;
		template <bool FirstCall=true, typename E, size_t NodeCount>
		void constructTreeFromLeafToRoot(const array<E, NodeCount>&);

		static Leaf* minLeaf(Base*);
		static Leaf* recurSelectNode(Base*, function<Base*(Middle*)>&);
		// Base* cloneNodes(Btree&) const;
        //void root_add(const node_instance*, const std::pair<Key, Value>&);
		//void create_new_branch(const node_instance*, const std::pair<Key, Value>&);
		//void create_new_root(const node_instance*, const std::pair<Key, Value>&);
		// void merge_branch(Key, const Base*); // for Base merge a branch
	};


}

namespace btree {
#define BTREE Btree<BtreeOrder, Key, Value>

	BTREE_TEMPLATE
	template <size_t NumOfArrayEle>
	BTREE::Btree(
		LessThan lessThan,
		array<pair<Key, Value>, NumOfArrayEle> pairArray
	) :
	_lessThanPtr(make_shared<LessThan>(lessThan))
	{
		if constexpr (NumOfArrayEle == 0) { return; }

		sort(pairArray.begin(),
			 pairArray.end(),
			 [&] (auto& p1, auto& p2) {
				 return lessThan(p1.first, p2.first);
			 });

		if constexpr (NumOfArrayEle <= BtreeOrder) {
			_root = make_unique<Leaf>(pairArray.begin(), pairArray.end(), _lessThanPtr);
		} else {
			constructTreeFromLeafToRoot(pairArray);
		}

		_keyNum += NumOfArrayEle;
	}

	BTREE_TEMPLATE
	template <size_t NumOfArrayEle>
	BTREE::Btree(
		LessThan lessThan,
		array<pair<Key, Value>, NumOfArrayEle>&& pairArray
	) : Btree(lessThan, pairArray)
	{ }

	BTREE_TEMPLATE
	template <bool FirstCall, typename E, size_t NodeCount>
	void
	BTREE::constructTreeFromLeafToRoot(const array<E, NodeCount>& nodesMaterial)
	{
		constexpr auto upperNodeNum = (NodeCount % BtreeOrder == 0) ? (NodeCount / BtreeOrder) : (NodeCount / BtreeOrder + 1);
		array<pair<Key, unique_ptr<Base>>, upperNodeNum> upperNodes;

		auto head = nodesMaterial.begin();
		auto end = nodesMaterial.end();
		auto i = 0;
		auto tail = head + BtreeOrder;
		auto firstLeaf = true;

		do {
			if constexpr (FirstCall) {
				auto leaf = make_unique<Leaf>(head, tail, _lessThanPtr);
				upperNodes[i] = make_pair(leaf->maxKey(), std::move(leaf));
				if (firstLeaf) {
					firstLeaf = false;
				} else {
					static_cast<Leaf*>(upperNodes[i - 1].second.get())->nextLeaf(leaf.get());
				}
			} else {
				auto middle = make_unique<Middle>(head, tail, _lessThanPtr);
				upperNodes[i] = make_pair(middle->maxKey(), std::move(middle));
			}

			head = tail;
			tail = (end - tail > BtreeOrder) ? (tail + BtreeOrder) : end;
			++i;
		} while (end - head > 0);

		if constexpr (upperNodeNum <= BtreeOrder) {
			_root = make_unique<Middle>(upperNodes.begin(), upperNodes.end(), _lessThanPtr);
		} else {
			constructTreeFromLeafToRoot<false>(upperNodes);
		}
	}


	BTREE_TEMPLATE
	BTREE::Btree(const Btree& that)
		: _keyNum(that._keyNum), _root(that._root->clone()), _lessThanPtr(that._lessThanPtr)
	{}

	BTREE_TEMPLATE
	BTREE::Btree(Btree&& that) noexcept
		: _keyNum(that._keyNum), _root(that._root.release()), _lessThanPtr(that._lessThanPtr)
	{ }

	BTREE_TEMPLATE
	BTREE&
	BTREE::operator=(const Btree& that)
	{
		_keyNum = that._keyNum;
		_root.reset(that._root->clone());
		_lessThanPtr = that._lessThanPtr;
	}

	BTREE_TEMPLATE
	BTREE&
	BTREE::operator=(Btree&& that) noexcept
	{
		this->_keyNum = that._keyNum;
		this->_root.reset(that._root.release());
		this->_lessThanPtr = that._lessThanPtr;
	}

	BTREE_TEMPLATE
	Value
	BTREE::search(const Key& key) const
	{
		auto valuePtr = _root->search(key);// if _root is nullptr, will return what?
		if (valuePtr == nullptr) {
			throw runtime_error("The key you searched is beyond the max key.");
		}

		return *valuePtr;
	}

	BTREE_TEMPLATE
	void
	BTREE::add(pair<Key, Value> pair)
	{
		if (have()) {
			_root.reset(new Leaf(&pair, &pair + 1, _lessThanPtr));
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
		if (!have(pair.first)) {
			throw runtime_error(
				"Unable to modify, because the tree has no that key-value");
		}

		auto& k = pair.first;
		auto& v = pair.second;
		
		*(_root->search(k)) = v;
	}

	BTREE_TEMPLATE
	vector<Key>
	BTREE::explore() const
	{
		vector<Key> keys;
		keys.reserve(_keyNum);

		traverseLeaf([&keys](Leaf* l) {
			auto&& ks = l->allKey();
			keys.insert(keys.end(), ks.begin(), ks.end());

			return false;
		});

		return keys;
	}

	BTREE_TEMPLATE
	void
	BTREE::remove(const Key& key)
	{
		if (!have(key)) {
			return;
		}

		_root->remove(key);
		--_keyNum;
	}

	BTREE_TEMPLATE
	bool
	BTREE::have(const Key& key) const
	{
	    if (have()) {
            return _root->have(key);
	    }

	    return false;
	}

	BTREE_TEMPLATE
	bool
	BTREE::have() const
	{
        return _root.get() != nullptr;
    }

	BTREE_TEMPLATE
	vector<typename BTREE::Leaf*>
	BTREE::traverseLeaf(function<bool(Leaf*)>& predicate) const
	{
		vector<Leaf*> leafCollection;

		if (!have()) {
		    return leafCollection;
		}
		auto current = minLeaf(_root.get()); // Maybe not use minLeaf to get this leaf, hack NodeBase::search
		while (current != nullptr) {
			if (predicate(current)) {
				leafCollection.emplace_back(current);
			}

			current = current->nextLeaf();
		}

		return leafCollection;
	}

	// BTREE_TEMPLATE
	// typename BTREE::Base*
	// BTREE::cloneNodes(Btree& newBtree) const
	// {
	// 	if (!_root->Middle) {
	// 		auto l = new Leaf(*(_root.get()->self()), newBtree);
	// 		return l;
	// 	}
    //
	// 	vector<Leaf*> old_leafs= traverseLeaf([](Leaf*) -> bool {
	// 		return true;
	// 	});
    //
	// 	vector<Base*> oldChilds{};
	// 	oldChilds.reserve(_keyNum);
	// 	for (auto& l: old_leafs) {
	// 		oldChilds.emplace_back(l);
	// 	}
    //
    //     vector<Base*> newChilds{};
    //     newChilds.reserve(_keyNum);
    //     Leaf* next = nullptr;
    //     for (auto b = oldChilds.rbegin(), e = oldChilds.rend(); b != e; ++b) {
    //         auto l = new Leaf(*b, newBtree, next);
    //         newChilds.emplace_back(l);
    //         next = l;
    //     }
    //
	// 	do {
	// 		auto i = 0;
	// 		vector<Base*> new_fathers{};
	// 		vector<Base*> old_fathers{};
    //
	// 		do {
	// 			auto child_num = oldChilds[i]->father()->child_count();
	// 			old_fathers.emplace(oldChilds[i]->father());
    //
	// 			new_fathers.emplace(new Middle(
	// 				newBtree,
	// 				newChilds.begin() + i,
	// 				newChilds.begin() + i + child_num
	// 			));
	//
	// 			i += child_num;
	// 		} while (i < oldChilds.size());
    //
	// 		// update
	// 		std::swap(newChilds, new_fathers);
	// 		std::swap(oldChilds, old_fathers);
	// 	} while (newChilds.size() > 1);
    //
	// 	return newChilds[0];
	// }

	BTREE_TEMPLATE
	typename BTREE::Leaf*
	BTREE::minLeaf(Base* node)
	{
        function<Base*(Middle*)> min = [] (auto n) {
            return n->minSon();
        };
        
		return recurSelectNode(node, min);
	}

	BTREE_TEMPLATE
    typename BTREE::Leaf*
	BTREE::recurSelectNode(Base* node, function<Base*(Middle*)>& choose)
	{
		while (node->Middle) {
			node = choose(static_cast<Middle*>(node));
		}

		return static_cast<Leaf*>(node);
	}

#undef BTREE
#undef BTREE_TEMPLATE
}

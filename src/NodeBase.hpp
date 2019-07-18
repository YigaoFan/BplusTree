#pragma once
#include <vector> // for vector
#include "Elements.hpp"

namespace btree {
	struct LeafFlag   {};
	struct MiddleFlag {};

#define NODE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>

	NODE_TEMPLATE
	class LeafNode;

	NODE_TEMPLATE
	class MiddleNode;

	NODE_TEMPLATE
	class NodeBase {
	public:
		using Ele      = Elements<Key, Value, BtreeOrder, NodeBase>;
		using LessThan = typename Ele::LessThan;
		const bool Middle;
		template <typename Iter>
		NodeBase(LeafFlag,   Iter, Iter, shared_ptr<LessThan>);
		template <typename Iter>
		NodeBase(MiddleFlag, Iter, Iter, shared_ptr<LessThan>);
		NodeBase(const NodeBase&);
		NodeBase(NodeBase&&) noexcept;
		virtual unique_ptr<NodeBase> clone() const = 0;
		virtual ~NodeBase() = default;

		inline Key         maxKey() const;
		uint16_t           childCount() const;
		inline vector<Key> allKey() const;
		// virtual NodeBase*  father() const;
		inline bool        have  (const Key&);
		Value*             search(const Key&);
		bool               add(pair<Key, Value>);
		bool               remove(const Key&);

	protected:
		// NodeBase* father_{ nullptr };
		Elements<Key, Value, BtreeOrder, NodeBase> elements_;		
		inline bool empty() const;
		inline bool full()  const;
	};
}

namespace btree {
#define BASE NodeBase<Key, Value, BtreeOrder>

	NODE_TEMPLATE
	template <typename Iter>
	BASE::NodeBase(LeafFlag, Iter begin, Iter end, shared_ptr<LessThan> lessThanPtr)
		: Middle(false), elements_(begin, end, lessThanPtr)
	{ }	
	NODE_TEMPLATE
	template <typename Iter>
	BASE::NodeBase(MiddleFlag, Iter begin, Iter end, shared_ptr<LessThan> funcPtr)
		: Middle(true), elements_(begin, end, funcPtr)
	{
		// 好像不需要设置 father 了
		for (; begin != end; ++begin) {
			// need to use SFINE to control the property below?
			// TODO error
			begin->second->father_ = this;
		}
	}

	NODE_TEMPLATE
	BASE::NodeBase(const NodeBase& that)
		: Middle(that.Middle), elements_(that.elements_)
	{ }

	NODE_TEMPLATE
	BASE::NodeBase(NodeBase&& that) noexcept
		: Middle(that.Middle), father_(that.father_), elements_(std::move(that.elements_))
	{ }


	// NODE_TEMPLATE
	// BASE*
	// BASE::father() const
	// {
	// 	return father_;
	// }

	NODE_TEMPLATE
	uint16_t
	BASE::childCount() const
	{
		return elements_.count();
	}

	NODE_TEMPLATE
	bool
	BASE::empty() const
	{
		return elements_.count() == 0;
	}	

	NODE_TEMPLATE
	bool
	BASE::full() const
	{
		return elements_.full();
	}

	NODE_TEMPLATE
	Key
	BASE::maxKey() const
	{
		return elements_[elements_.count() - 1].first;
	}

	NODE_TEMPLATE
	bool
	BASE::have(const Key& key)
	{
		if (!Middle) {
			return elements_.have(key);
		} else {
			for (auto& e : elements_) {
				if ((*elements_.LessThanPtr)(key, e.first)) {
					Ele::ptr(e.second)->have(key);
				} else if (!(*elements_.LessThanPtr)(e.first, key)) {
					return true;
				}
			}

			return false;
		}
	}

	NODE_TEMPLATE
	Value*
	BASE::search(const Key& key)
	{
		auto maxValueForContent = [] (Ele& e) {
			return e[e.count() - 1].second;
		};

		if (!Middle) {
			return elements_.have(key) ? &Ele::value(elements_[key]) : nullptr;
		} else {
			for (auto& e : elements_) {
				if ((*elements_.LessThanPtr)(key, e.first)) {
					return Ele::ptr(e.second)->search(key);
				} else if (!(*elements_.LessThanPtr)(e.first, key)) {
					auto subNodePtr = Ele::ptr(e.second);

				SearchInThisNode:
					if (!subNodePtr->Middle) {
						return &Ele::value(maxValueForContent(subNodePtr->elements_));
					} else {
						subNodePtr = Ele::ptr(maxValueForContent(subNodePtr->elements_));
						goto SearchInThisNode;
					}
				}
			}

			return nullptr;
		}
	}

	NODE_TEMPLATE
	vector<Key>
	BASE::allKey() const
	{
		vector<Key> keys;
		keys.reserve(elements_.count());
		for (auto& e : elements_) {
			keys.emplace_back(e.first);
		}

		return keys;
	}

	NODE_TEMPLATE
	unique_ptr<BASE>
	BASE::clone() const
	{
		throw runtime_error("NodeBase's clone is invalid.");
	}

	NODE_TEMPLATE
	bool
	BASE::add(pair<Key, Value> p)
	{
		using LeafNode   = LeafNode<Key, Value, BtreeOrder>;
		using MiddleNode = MiddleNode<Key, Value, BtreeOrder>;

		vector<decltype(this)> passedNodeTrackStack;
		passedNodeTrackStack.push_back(this);

		if (!Middle) {
			static_cast<LeafNode*>(this)->add(p, passedNodeTrackStack);
		} else {
			static_cast<MiddleNode*>(this)->add(p, passedNodeTrackStack);
		}
		// TODO profile between using if to call different function and using virtual function
	}

	NODE_TEMPLATE
	bool
	BASE::remove(const Key& key)
	{
		auto maxValue = [] (Ele& e) -> typename Ele::ValueForContent& {
			return e[e.count() - 1].second;
		};

		// Keep internal node key count between w/2 and w
		if (!Middle) {
			elements_.remove(key);
			// check count
		} else {
			for (auto& e : elements_) {
				if ((*elements_.LessThanPtr)(key, e.first)) {
					// Recursive locate
				} else if (!(*elements_.LessThanPtr)(e.first, key)) {
					auto subNodePtr = Ele::ptr(e.second);

				SearchInThisNode:
					if (!subNodePtr->Middle) {
						subNodePtr->elements_.remove(key);
						// Check count
					} else {
						subNodePtr = Ele::ptr(maxValue(subNodePtr->elements_));
						goto SearchInThisNode;
					}
				}
			}
		}
	}
#undef BASE
#undef NODE_TEMPLATE
}

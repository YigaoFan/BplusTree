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
		virtual NodeBase*  father() const;
		inline bool        have  (const Key&);
		Value*             search(const Key&);
		bool               add(pair<Key, Value>);
		bool               remove(const Key&);

	protected:
		NodeBase* father_{ nullptr };
		Elements<Key, Value, BtreeOrder, NodeBase> elements_;		
		inline bool empty() const;
		inline bool full()  const;
		inline void addBeyondMax(pair<Key, Value>);
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


	NODE_TEMPLATE
	BASE*
	BASE::father() const
	{
		return father_;
	}

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
#define MAX_ELE [elements_.count() - 1]
		using Ele::ptr;
		using Leaf = LeafNode<Key, Value, BtreeOrder>;
		// Keep internal node key count between w/2 and w
		auto& k = p.first;
		auto& lessThan = *(elements_.LessThanPtr);

		// TODO could maintain a stack to store recursive record
		if (!Middle) {
			static_cast<Leaf*>(this)->add(p);
	    } else {
			for (auto& e : elements_) {
				if (lessThan(k, e.first)) {
					auto subNodePtr = ptr(e.second);
					subNodePtr->add(p);
				}
			}

			addBeyondMax(p);
	    }
    }

    // TODO how to make all _elements of Element in a vector?
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

	// Name maybe not very accurate, because not must beyond
	NODE_TEMPLATE
	void
	BASE::addBeyondMax(pair<Key, Value> p)
	{
		// Ptr append will some thing different
		if (!MAX_ELE->full()) {
			MAX_ELE.append(p);
		} else if (/*sibling space free*/) {
			// move some element to sibling
		} else {
			// split node into two
		}
		// Think about this way: should add new right most node to expand max bound
#undef MAX_ELE
	}

#undef BASE
#undef NODE_TEMPLATE
}

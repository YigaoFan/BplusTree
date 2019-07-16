#pragma once
#include <vector> // for vector
#include "Elements.hpp"

namespace btree {
	struct LeafFlag   {};
	struct MiddleFlag {};

#define NODE_BASE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>

	NODE_BASE_TEMPLATE
	class NodeBase {
	public:
		using Ele      = Elements<Key, Value, BtreeOrder, NodeBase>;
		using LessThan = typename Ele::LessThan;
		const bool Middle		
		template <typename Iter>
		NodeBase(LeafFlag,   Iter, Iter, shared_ptr<LessThan>);
		template <typename Iter>
		NodeBase(MiddleFlag, Iter, Iter, shared_ptr<LessThan>);
		NodeBase(const NodeBase&);
		NodeBase(NodeBase&&) noexcept;
		virtual unique_ptr<NodeBase> clone() const = 0;
		virtual ~NodeBase() = default		
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
		inline bool siblingSpaceFree() const;
		inline void siblingElementsReallocate(pair<Key, Value>);
		inline void splitNode(pair<Key, Value>);
		inline void addBeyondMax(pair<Key, Value>);
    };
}

namespace btree {
#define NODE NodeBase<Key, Value, BtreeOrder>

	NODE_BASE_TEMPLATE
	template <typename Iter>
	NODE::NodeBase(LeafFlag, Iter begin, Iter end, shared_ptr<LessThan> lessThanPtr)
		: Middle(false), elements_(begin, end, lessThanPtr)
	{ }	
	NODE_BASE_TEMPLATE
	template <typename Iter>
	NODE::NodeBase(MiddleFlag, Iter begin, Iter end, shared_ptr<LessThan> funcPtr)
		: Middle(true), elements_(begin, end, funcPtr)
	{
		for (; begin != end; ++begin) {
			// need to use SFINE to control the property below?
			// TODO error
			begin->second->father_ = this;
		}
	}

	NODE_BASE_TEMPLATE
	NODE::NodeBase(const NodeBase& that)
		: Middle(that.Middle), elements_(that.elements_)
	{ }

	NODE_BASE_TEMPLATE
	NODE::NodeBase(NodeBase&& that) noexcept
		: Middle(that.Middle), father_(that.father_), elements_(std::move(that.elements_))
	{ }


	NODE_BASE_TEMPLATE
	NODE*
	NODE::father() const
	{
		return father_;
	}

	NODE_BASE_TEMPLATE
	uint16_t
	NODE::childCount() const
	{
		return elements_.count();
	}

	NODE_BASE_TEMPLATE
	bool
	NODE::empty() const
	{
		return elements_.count() == 0;
	}	

	NODE_BASE_TEMPLATE
	bool
	NODE::full() const
	{
		return elements_.full();
	}

	NODE_BASE_TEMPLATE
    Key
    NODE::maxKey() const
    {
        return elements_[elements_.count() - 1].first;
    }

	NODE_BASE_TEMPLATE
	bool
	NODE::have(const Key& key)
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

	NODE_BASE_TEMPLATE
	Value*
	NODE::search(const Key& key)
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

	NODE_BASE_TEMPLATE
	vector<Key>
	NODE::allKey() const
	{
		vector<Key> keys;
		keys.reserve(elements_.count());
		for (auto& e : elements_) {
			keys.emplace_back(e.first);
		}

		return keys;
    }

	NODE_BASE_TEMPLATE
	unique_ptr<NODE>
	NODE::clone() const
	{
		throw runtime_error("NodeBase's clone is invalid.");
	}

	NODE_BASE_TEMPLATE
	bool
	NODE::add(pair<Key, Value> p)
	{
#define LAST_ELE elements_[elements_.count() - 1]
		using Ele::ptr;
		// Keep internal node key count between w/2 and w
		auto& k = p.first;
		auto& v = p.second;
		auto& lessThan = *(elements_.LessThanPtr);

		if (!Middle) {
			if (!full()) {
				if (lessThan(k, LAST_ELE.first)) {
					elements_.insert(p);
				} else {
					elements_.append(p);
					// change bound in NodeBase above
				}
			} else if (siblingSpaceFree()) {
				siblingElementsReallocate(p);
			} else {
				splitNode(p);
			}
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
	NODE_BASE_TEMPLATE
	bool
	NODE::remove(const Key& key)
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
	NODE_BASE_TEMPLATE
	void
	NODE::addBeyondMax(pair<Key, Value> p)
	{
		// Ptr append will some thing different
		if (!LAST_ELE.full()) {
			LAST_ELE.append(p);
		} else if (/*sibling space free*/) {
			// move some element to sibling
		} else {
			// split node into two
		}
		// Think about this way: should add new right most node to expand max bound
#undef LAST_ELE
	}

	NODE_BASE_TEMPLATE
	bool
	NODE::siblingSpaceFree() const
	{
		// TODO
		// Just ask left and right LeafNode if it has free space
		// TODO If you want to fine all the node, you could leave a gap "fineAllocate" to fine global allocate
		return false;
	}

	NODE_BASE_TEMPLATE
	void
	NODE::siblingElementsReallocate(pair<Key, Value> p)
	{
		// if max change, need to change above
	}

	NODE_BASE_TEMPLATE
	void
	NODE::splitNode(pair<Key, Value> p)
	{
		// if max change, need to change above
	}
#undef NODE
#undef NODE_BASE_TEMPLATE
}

#pragma once
#include "NodeBaseCrtp.hpp"

namespace btree {
#define NODE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>
#define MIDDLE MiddleNode<Key, Value, BtreeOrder>

	NODE_TEMPLATE
	class MiddleNode : public NodeBase_CRTP<MIDDLE, Key, Value, BtreeOrder> {
	private:
		using          Base = NodeBase<Key, Value, BtreeOrder>;
		using          Base_CRTP = NodeBase_CRTP<MIDDLE, Key, Value, BtreeOrder>;
		using typename Base::LessThan;

	public:
		template <typename Iter>
		MiddleNode(Iter, Iter, shared_ptr<LessThan>);
		MiddleNode(const MiddleNode&);
		MiddleNode(MiddleNode&&) noexcept;
		~MiddleNode() override = default;

		Base* minSon() const;
		void  searchPrevious(const vector<Base *> &, Base *&) const;
		void  searchNext    (const vector<Base *> &, Base *&) const;

	private:
		Base* maxSon() const;
	};
}

namespace btree {
	NODE_TEMPLATE
	template <typename Iter>
	MIDDLE::MiddleNode(Iter begin, Iter end, shared_ptr<LessThan> funcPtr)
		: Base_CRTP(begin, end, funcPtr)
	{ }

	NODE_TEMPLATE
	MIDDLE::MiddleNode(const MiddleNode& that)
		: Base_CRTP(that)
	{ }

	NODE_TEMPLATE
	MIDDLE::MiddleNode(MiddleNode&& that) noexcept
		: Base_CRTP(std::move(that))
	{ }

	/**
	 * TODO should check exist at least one
	 */
	NODE_TEMPLATE
	typename MIDDLE::Base*
	MIDDLE::minSon() const
	{
		auto& es = Base::elements_;

		return Base::Ele::ptr(es[0].second);
	}

	NODE_TEMPLATE
	typename MIDDLE::Base*
	MIDDLE::maxSon() const
	{
		auto& es = Base::elements_;

		return Base::Ele::ptr(es[this->childCount()-1].second);
	}

#define SEARCH_HELPER_DEF(FUN_NAME, COMPARE_TO_BOUND, OFFSET, CHOOSE_SON) \
	function<Base*(decltype(rIter))> FUN_NAME = [&] (decltype(rIter) currentNodeIter) -> Base* { \
		auto upperNodeIter = ++rIter;                                                            \
		if (upperNodeIter == rEnd) {                                                             \
			return nullptr;                                                                      \
		}                                                                                        \
    	                                                                                         \
		auto& upperElements = upperNodeIter->elements_;                                          \
		auto i = upperElements.indexOf(*currentNodeIter);                                        \
		                                                                                         \
		if (i COMPARE_TO_BOUND) {                                                                \
			return Base::Ele::ptr(upperElements[i OFFSET].second);                               \
		} else {                                                                                 \
			auto previousOfUpper = searchPreHelper(upperNodeIter);                               \
			return static_cast<MiddleNode*>(previousOfUpper)->CHOOSE_SON();                      \
		}                                                                                        \
	};

	/**
	 * not change the stack
	 */
	NODE_TEMPLATE
	void
	MIDDLE::searchPrevious(const vector<Base *> &passedNodeTrackStack, Base *&previous) const
	{
		auto& stack = passedNodeTrackStack;
		auto rIter = stack.rbegin();
		auto rEnd  = stack.rend();

		SEARCH_HELPER_DEF(searchPreHelper, >0, -1, maxSon)
		previous = searchPreHelper(rIter);
	}

	/**
	 * not change the stack
	 */
	NODE_TEMPLATE
	void
	MIDDLE::searchNext(const vector<Base *> &passedNodeTrackStack, Base *&next) const
	{
		auto& stack = passedNodeTrackStack;
		auto rIter = stack.rbegin();
		auto rEnd  = stack.rend();

		SEARCH_HELPER_DEF(searchNxtHelper, <(upperNodeIter->childCount()-1), +1, minSon)
		next = searchNxtHelper(rIter);
	}
#undef SEARCH_HELPER_DEF

#undef MIDDLE
#undef NODE_TEMPLATE
}
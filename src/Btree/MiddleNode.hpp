#pragma once
#include "NodeBaseCrtp.hpp"

namespace Collections 
{
	using ::std::move;

#define NODE_TEMPLATE 
#define MIDDLE MiddleNode<Key, Value, BtreeOrder>

	template <typename Key, typename Value, uint16_t BtreeOrder>
	class MiddleNode : public NodeBase_CRTP<MIDDLE, Key, Value, BtreeOrder> 
	{
	private:
		using          Base = NodeBase<Key, Value, BtreeOrder>;
		using          Base_CRTP = NodeBase_CRTP<MIDDLE, Key, Value, BtreeOrder>;
		using typename Base::LessThan;

	public:
		template <typename Iter>
		MiddleNode(Iter begin, Iter end, shared_ptr<LessThan> lessThanPtr)
			: Base_CRTP(begin, end, lessThanPtr)
		{ }

		template <typename Iterator>
		MiddleNode(Enumerator<pair<Key, Value>, Iterator> enumerator, shared_ptr<LessThan> lessThanPtr)
			: Base_CRTP(enumerator, lessThanPtr)
		{}

		MiddleNode(const MiddleNode& that)
			: Base_CRTP(that)
		{ }

		MiddleNode(MiddleNode&& that) noexcept
			: Base_CRTP(::std::move(that))
		{ }

		~MiddleNode() override = default;

		Base* MinSon() const
		{
			auto& es = Base::elements_;
			return Base::Ele::ptr(es[0].second);
		}

#define SEARCH_HELPER_DEF(FUN_NAME, COMPARE_TO_BOUND, OFFSET, CHOOSE_SON)                                                                                 \
	function<Base*(decltype(rIter), function<Base*(Base*)>)> FUN_NAME = [&] (decltype(rIter) currentNodeIter, function<Base*(Base*)> callBack) -> Base* { \
        auto upperNodeIter = ++rIter;                                                                                                                     \
        if (upperNodeIter == rEnd)                                                                                                                        \
		{                                                                                                                                                 \
            return callBack(nullptr);                                                                                                                     \
        }                                                                                                                                                 \
                                                                                                                                                          \
        auto& upperElements = static_cast<MiddleNode*>(ptrOff(upperNodeIter))->elements_;                                                                 \
        auto i = upperElements.IndexOf(*currentNodeIter);                                                                                                 \
                                                                                                                                                          \
        if (i COMPARE_TO_BOUND) {                                                                                                                         \
            return callBack(Base::Ele::ptr(upperElements[i OFFSET].second));                                                                              \
        } else {                                                                                                                                          \
            return FUN_NAME(upperNodeIter, [callBack{ std::move(callBack) }] (Base* siblingOfUpper) {                                                     \
                return callBack(static_cast<MiddleNode*>(siblingOfUpper)->CHOOSE_SON());                                                                  \
        	});                                                                                                                                           \
    	}                                                                                                                                                 \
	}

		void searchPrevious(const vector<Base *> &passedNodeTrackStack, Base *&previous) const
		{
			auto& stack = passedNodeTrackStack;
			auto rIter = stack.rbegin();
			auto rEnd = stack.rend();

			SEARCH_HELPER_DEF(searchPreHelper, > 0, -1, maxSon);
			previous = searchPreHelper(rIter, [](auto n) { return n; });
		}

		void searchNext(const vector<Base *> &passedNodeTrackStack, Base *&next) const
		{
			auto& stack = passedNodeTrackStack;
			auto rIter = stack.rbegin();
			auto rEnd = stack.rend();

			SEARCH_HELPER_DEF(searchNxtHelper, < (static_cast<MiddleNode*>(ptrOff(upperNodeIter))->ChildCount() - 1),
							  +1, MinSon);
			next = searchNxtHelper(rIter, [](auto n) { return n; });
		}
#undef SEARCH_HELPER_DEF

	private:
		Base* maxSon() const
		{
			auto& es = Base::elements_;
			return Base::Ele::ptr(es[this->ChildCount() - 1].second);
		}
	};
}
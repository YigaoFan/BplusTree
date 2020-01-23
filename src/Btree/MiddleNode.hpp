#pragma once
#include <memory>
#include "NodeBaseCrtp.hpp"

namespace Collections 
{
	using ::std::move;
	using ::std::unique_ptr;

#define NODE_TEMPLATE 
#define MIDDLE MiddleNode<Key, Value, BtreeOrder>

	template <typename Key, typename Value, uint16_t BtreeOrder>
	class MiddleNode : public NodeBase_CRTP<MIDDLE, Key, Value, BtreeOrder> 
	{
	private:
		using          Base = NodeBase<Key, Value, BtreeOrder>;
		using          Base_CRTP = NodeBase_CRTP<MIDDLE, Key, Value, BtreeOrder>;
		using typename Base::LessThan;
		Elements<Key, unique_ptr<Base>, BtreeOrder - 1> _elements;

	public:
		template <typename Iterator, typename T>
		MiddleNode(Enumerator<pair<Key, T>, Iterator> enumerator, shared_ptr<LessThan> lessThanPtr)
			: Base_CRTP(), _elements(enumerator, lessThan)
		{
			// TODO args should be a list of NodeBase pointer 
			// and use LeafNodes' raw key to cons ref Key 
			// and use MiddleNodes' ref key to cons ref Key 
		}

		MiddleNode(MiddleNode const& that)
			: Base_CRTP(that)
		{ }

		MiddleNode(MiddleNode&& that) noexcept
			: Base_CRTP(move(that))
		{ }

		~MiddleNode() override = default;
		
		vector<Key> Keys() const override
		{
			return MinSon()->Keys();
		}

		bool Middle() const override
		{
			return true;
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

			SEARCH_HELPER_DEF(searchPreHelper, > 0, -1, MaxSon);
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
		Base* MaxSon() const
		{
			auto& es = Base::elements_;
			return Base::Ele::ptr(es[this->ChildCount() - 1].second);
		}

		Base* MinSon() const
		{
			auto& es = this->elements_;
			return Base::Ele::ptr(es[0].second);
		}
	};
}
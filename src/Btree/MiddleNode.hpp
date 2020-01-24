#pragma once
#include <memory>
#include "Elements.hpp"
#include "NodeBaseCrtp.hpp"

namespace Collections 
{
	using ::std::move;
	using ::std::unique_ptr;
	using ::std::cref;
	using ::std::pair;
	using ::std::make_pair;

#define MIDDLE MiddleNode<Key, Value, BtreeOrder>

	template <typename Key, typename Value, order_int BtreeOrder>
	class MiddleNode : public NodeBase_CRTP<MIDDLE, Key, Value, BtreeOrder>
#undef MIDDLE
	{
	private:
		using          Base = NodeBase<Key, Value, BtreeOrder>;
		using          Base_CRTP = NodeBase_CRTP<MiddleNode, Key, Value, BtreeOrder>;
		using typename Base::LessThan;
		Elements<cref<Key>, unique_ptr<Base>, BtreeOrder> _elements;

		static pair<cref<Key>, unique_ptr<Base>> ConvertToKeyBasePtrPair(unique_ptr<Base> node)
		{
			return make_pair<cref<Key>, unique_ptr<Base>>(cref(node->MinKey()), move(node));
		}

	public:
		template <typename Iterator>
		MiddleNode(Enumerator<unique_ptr<Base>, Iterator> enumerator, shared_ptr<LessThan> lessThanPtr)
			: Base_CRTP(), _elements(EnumeratorPipeine(enumerator, ConvertToKeyBasePtrPair), lessThan)
		{ }

		MiddleNode(MiddleNode const& that)
			: Base_CRTP(that)// TODO how to solve _elements copy
		{ }

		MiddleNode(MiddleNode&& that) noexcept
			: Base_CRTP(move(that)), _elements(move(that._elements))
		{ }

		~MiddleNode() override = default;

		bool Middle() const override
		{
			return true;
		}

		vector<Key> Keys() const override
		{
			return MinSon()->Keys();
		}

		Key const& MinKey() override const
		{
			return _elements[0].first;
		}

		void Add(pair<Key, Value> p) override
		{
			auto i = _elements.SuitPosition<true>(p.first);
			_elements[i].second->Add(move(p));
		}

		virtual void Remove(Key const& key) override
		{
			auto i = _elements.SuitPosition<true>(p.first);
			_elements[i].second->Remove(key);
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
			return _elements[this->ChildCount() - 1].second;
		}

		Base* MinSon() const
		{
			return _elements[0].second;
		}

		// TODO maybe add and remove are in the same method
		void AddBelowNodeCallback(Base* srcNode, Base* newNextNode)
		{
			// find index of srcNode and add new NextNode
			// if this is Full(), combine the node or call the upper node callback
		}
	};
}
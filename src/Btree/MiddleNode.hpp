#pragma once
#include <memory>
#include <functional>
#include "Basic.hpp"
#include "Elements.hpp"
#include "NodeBaseCrtp.hpp"

namespace Collections 
{
	using ::std::move;
	using ::std::unique_ptr;
	using ::std::cref;
	using ::std::reference_wrapper;
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
		using _LessThan = LessThan<Key>;
		Elements<reference_wrapper<Key>, unique_ptr<Base>, BtreeOrder> _elements;

		static pair<reference_wrapper<Key>, unique_ptr<Base>> ConvertToKeyBasePtrPair(unique_ptr<Base> node)
		{
			return make_pair<reference_wrapper<Key>, unique_ptr<Base>>(cref(node->MinKey()), move(node));
		}

	public:
		template <typename Iterator>
		MiddleNode(Enumerator<unique_ptr<Base>, Iterator> enumerator, shared_ptr<_LessThan> lessThanPtr)
			: Base_CRTP(), _elements(EnumeratorPipeine(enumerator, ConvertToKeyBasePtrPair), lessThanPtr)
		{
			for (auto& e : _elements)
			{
				e.second.SetUpNodeCallback(AddSubNodeCallback, DeleteSubNodeCallback);
			}
		}

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

		Key const& MinKey() const override
		{
			return _elements[0].first;
		}

		bool ContainsKey(Key const&) const override
		{
			throw NotImplementException();
		}

		Value GetValue(Key const& key) const override
		{
			throw NotImplementException();
		}

		void ModifyValue(Key const& key, Value value) override
		{
			throw NotImplementException();
		}

		void Add(pair<Key, Value> p) override
		{
			auto i = _elements.SuitPosition<true>(p.first);
			_elements[i].second->Add(move(p));
		}

		void Remove(Key const& key) override
		{
			auto k = key;// TODO how to convert reference type to reference_wrapper type
			auto i = _elements.SuitPosition<true>(k/*ey*/);
			_elements[i].second->Remove(key);
		}

//#define SEARCH_HELPER_DEF(FUN_NAME, COMPARE_TO_BOUND, OFFSET, CHOOSE_SON)                                                                                 \
//	function<Base*(decltype(rIter), function<Base*(Base*)>)> FUN_NAME = [&] (decltype(rIter) currentNodeIter, function<Base*(Base*)> callBack) -> Base* { \
//        auto upperNodeIter = ++rIter;                                                                                                                     \
//        if (upperNodeIter == rEnd)                                                                                                                        \
//		{                                                                                                                                                 \
//            return callBack(nullptr);                                                                                                                     \
//        }                                                                                                                                                 \
//                                                                                                                                                          \
//        auto& upperElements = static_cast<MiddleNode*>(ptrOff(upperNodeIter))->elements_;                                                                 \
//        auto i = upperElements.IndexOf(*currentNodeIter);                                                                                                 \
//                                                                                                                                                          \
//        if (i COMPARE_TO_BOUND) {                                                                                                                         \
//            return callBack(Base::Ele::ptr(upperElements[i OFFSET].second));                                                                              \
//        } else {                                                                                                                                          \
//            return FUN_NAME(upperNodeIter, [callBack{ std::move(callBack) }] (Base* siblingOfUpper) {                                                     \
//                return callBack(static_cast<MiddleNode*>(siblingOfUpper)->CHOOSE_SON());                                                                  \
//        	});                                                                                                                                           \
//    	}                                                                                                                                                 \
//	}
//
//		void searchPrevious(const vector<Base *> &passedNodeTrackStack, Base *&previous) const
//		{
//			auto& stack = passedNodeTrackStack;
//			auto rIter = stack.rbegin();
//			auto rEnd = stack.rend();
//
//			SEARCH_HELPER_DEF(searchPreHelper, > 0, -1, MaxSon);
//			previous = searchPreHelper(rIter, [](auto n) { return n; });
//		}
//
//		void searchNext(const vector<Base *> &passedNodeTrackStack, Base *&next) const
//		{
//			auto& stack = passedNodeTrackStack;
//			auto rIter = stack.rbegin();
//			auto rEnd = stack.rend();
//
//			SEARCH_HELPER_DEF(searchNxtHelper, < (static_cast<MiddleNode*>(ptrOff(upperNodeIter))->ChildCount() - 1),
//							  +1, MinSon);
//			next = searchNxtHelper(rIter, [](auto n) { return n; });
//		}
//#undef SEARCH_HELPER_DEF
	protected:
		// TODO this virtual method can be private or public?
//		template <RetValue ReturnValue, typename T>
//		auto FindHelper(Key const& key, function<T(*)> onEqualDo) override
//		{
//#define ON_NOT_FOUND if constexpr (ReturnValue == RetValue::Bool) { return false; } else { throw KeyNotFoundException(); }
//
//			if (order_int i = 0; auto result = _elements.TryIndexOf(key, i) == FindResult::MaybeExistInDeep)
//			{
//				_elements[i].second.FindHelper(key, move(onEqualDo));
//			}
//			else if (result == FindResult::EqualFound)
//			{
//				onEqualDo(_elements[i].second);
//			}
//			else
//			{
//				ON_NOT_FOUND;
//			}
//#undef ON_NOT_FOUND
//		}

	private:
		//Base* MaxSon() const
		//{
		//	return _elements[this->ChildCount() - 1].second;
		//}

		Base* MinSon() const
		{
			return _elements[0].second.get();
		}

		// TODO maybe add and remove are in the same method
		void AddSubNodeCallback(Base* srcNode, unique_ptr<Base> newNextNode)
		{
			// find index of srcNode and add new NextNode
			// if this is Full(), combine the node or call the upper node callback
		}

		void DeleteSubNodeCallback(Base* node)
		{
			// TODO
		}
	};
}
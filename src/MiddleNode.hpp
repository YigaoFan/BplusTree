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
		~MiddleNode() override;

		// TODO Base is not past Base, maybe occur some problem
		Base*            minSon();
		void             collectAddInfo(const Key&, vector<Base*>&);
		Base*            operator[](const Key&);
		pair<Key, Base*> operator[](uint16_t);

	private:
		// void addBeyondMax(pair<Key, Value>);
	};
}

namespace btree {
	NODE_TEMPLATE
	template <typename Iter>
	MIDDLE::MiddleNode(Iter begin, Iter end, shared_ptr<LessThan> funcPtr)
		: Base_CRTP(MiddleFlag(), begin, end, funcPtr)
	{ }

	NODE_TEMPLATE
	MIDDLE::MiddleNode(const MiddleNode& that)
		: Base_CRTP(that)
	{ }

	NODE_TEMPLATE
	MIDDLE::MiddleNode(MiddleNode&& that) noexcept
		: Base_CRTP(std::move(that))
	{ }

	NODE_TEMPLATE
	MIDDLE::~MiddleNode() = default;

	NODE_TEMPLATE
	typename MIDDLE::Base*
	MIDDLE::minSon()
	{
		auto& es = Base::elements_;

		return Base::Ele::ptr(es[0].second);
	}

	NODE_TEMPLATE
	typename MIDDLE::Base*
	MIDDLE::operator[](const Key& key)
	{
		auto& e = this->elements_[key];
		return Base::Ele::ptr(e);
	}

	NODE_TEMPLATE
	pair<Key, typename MIDDLE::Base*>
	MIDDLE::operator[](uint16_t i)
	{
		auto& e = this->elements_[i];
		// TODO because this is pass by value, can't be used by directly
		return make_pair(e.first, Base::Ele::ptr(e.second));
	}

	NODE_TEMPLATE
	void
	MIDDLE::collectAddInfo(const Key& key, vector<Base*>& passedNodeTrackStack)
	{
		//using Base::elements_;
		// auto& pointer  = Base::elements_.ptr;
		//using Base::Ele::ptr;
		auto& elements = Base::elements_;
		auto& lessThan = *(elements.LessThanPtr);
		auto& stack    = passedNodeTrackStack;

		stack.push_back(this);

#define KEY_OF_ELE e.first
#define VALUE_OF_ELE e.second
#define LAST_ONE (count - 1)
#define PTR(E) Base::Ele::ptr(E)

		auto count = elements.count();
		for (auto i = 0; i < count; ++i) {
			auto& e = elements[i];

			if (lessThan(key, KEY_OF_ELE) || i == LAST_ONE) { // Last one is always last one
				auto subNodePtr = PTR(VALUE_OF_ELE);

				if (subNodePtr->Middle) {
					static_cast<MiddleNode*>(subNodePtr)->collectAddInfo(key, stack);
				} else {
					stack.push_back(subNodePtr);
				}

				return;
			}
		}

#undef PTR
#undef LAST_ONE
#undef VALUE_OF_ELE
#undef KEY_OF_ELE
	}

	// TODO have problem, think about this method
	// Name maybe not very accurate, because not must beyond
	// NODE_TEMPLATE
	// void
	// MIDDLE::addBeyondMax(pair<Key, Value> p)
	// {
	// 	using Base::elements_;
	// 	using Base::Ele::ptr;
	// 	// Ptr append will some thing different
	// 	if (!Base::full()) {
	// 		elements_.append(p);
	// 		// update bound related info
	// 	} else if (/*sibling space free*/) {
	// 		// move some element to sibling
	// 	} else {
	// 		// split node into two
	// 	}
	// 	// Think about this way: should add new right most node to expand max bound
	//
	// }

#undef MIDDLE
#undef NODE_TEMPLATE
}
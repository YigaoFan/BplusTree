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

		Base*            minSon();
		Base*            operator[](const Key&);
		pair<Key, Base*> operator[](uint16_t);
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

#undef MIDDLE
#undef NODE_TEMPLATE
}
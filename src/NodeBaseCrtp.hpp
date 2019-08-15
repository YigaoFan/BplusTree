#pragma once
#include "NodeBase.hpp"

namespace btree {
	template<typename Derived, typename Key, typename Value, uint16_t BtreeOrder>
	class NodeBase_CRTP : public NodeBase<Key, Value, BtreeOrder> {
	protected:
		using Base = NodeBase<Key, Value, BtreeOrder>;

	public:
		// provide same constructors of NodeBase
		using Base::Base;

		// could return Derived type, but unique_ptr not allow
		virtual unique_ptr<Base> clone() const
		{
			// how to reduce the const mark
			return unique_ptr<Base>{ make_unique<Derived>(static_cast<Derived const &>(*this)).get() };
			// wait to verify
		}

		virtual unique_ptr<Base> move() const
		{
			// how to reduce the const mark
			return unique_ptr<Base>{ make_unique<Derived>(std::move(static_cast<Derived const &>(*this))).get() };
			// wait to verify
		}
	};
}

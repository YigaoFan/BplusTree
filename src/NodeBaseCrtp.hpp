#pragma once
#include "NodeBase.hpp"

namespace btree {
	template<typename Derived, typename Key, typename Value, uint16_t BtreeOrder>
	class NodeBase_CRTP : public NodeBase<Key, Value, BtreeOrder> {
	private:
		using Base = NodeBase<Key, Value, BtreeOrder>;

	public:
		// provide same constructor of NodeBase
		using Base::Base;

		// could return Derived type?, I think
		virtual unique_ptr<Base> clone() const
		{
			return make_unique<Base>(*(new Derived(*this)));
		}
	};
}

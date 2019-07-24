#pragma once
#include "NodeBase.hpp"

namespace btree {
	template<typename Derived, typename Key, typename Value, uint16_t BtreeOrder>
	class NodeBase_CRTP : public NodeBase<Key, Value, BtreeOrder> {
	private:
		using Base = NodeBase<Key, Value, BtreeOrder>;

	public:
		// TODO should provide same constructor of NodeBase

		virtual unique_ptr<Base> clone() const
		{
			// maybe not right, this class how to know Derived
			return make_unique<Base>(*(new Derived(*this)));
		}
	};
}

#pragma once
#include "Basic.hpp"
#include "NodeBase.hpp"

namespace Collections 
{
	using ::std::move;

	template<typename Derived, typename Key, typename Value, order_int BtreeOrder>
	class NodeBase_CRTP : public NodeBase<Key, Value, BtreeOrder>
	{
	protected:
		using Base = NodeBase<Key, Value, BtreeOrder>;

	public:
		// provide same constructors of NodeBase
		using Base::Base;

		// could return Derived type, but unique_ptr not allow
		virtual unique_ptr<Base> Clone() const
		{
			// how to reduce the const mark
			return unique_ptr<Base>{ make_unique<Derived>(static_cast<Derived const &>(*this)).get() };
		}

		// TODO this method ref exist?
		virtual unique_ptr<Base> Move() const
		{
			// how to reduce the const mark
			return unique_ptr<Base>{ make_unique<Derived>(move(static_cast<Derived const &>(*this))).get() };
		}
	};
}

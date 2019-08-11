#pragma once
#include <utility>
#include "NodeBaseCrtp.hpp"

namespace btree {
#define NODE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>
#define LEAF LeafNode<Key, Value, BtreeOrder>

	NODE_TEMPLATE
	class LeafNode : public NodeBase_CRTP<LEAF, Key, Value, BtreeOrder> {
		using Base       = NodeBase<Key, Value, BtreeOrder>;
		using Base_CRTP  = NodeBase_CRTP<LEAF, Key, Value, BtreeOrder>;
		using typename Base::LessThan;

	public:
		template <typename Iterator>
		LeafNode(Iterator, Iterator, shared_ptr<LessThan>);
		LeafNode(const LeafNode&, LeafNode* previous=nullptr,LeafNode* next=nullptr);
		LeafNode(LeafNode&&) noexcept;
		~LeafNode() override = default;

		const Value&     operator[](const Key&);
		pair<Key, Value> operator[](uint16_t);
		inline LeafNode* nextLeaf() const;
		inline void      nextLeaf(LeafNode*);
		inline LeafNode* previousLeaf() const;
		inline void      previousLeaf(LeafNode*);

	private:
		// TODO remember to initialize two property below
		LeafNode* _next    { nullptr };
		LeafNode* _previous{ nullptr };
	};
}

namespace btree {
	NODE_TEMPLATE
	template <typename Iter>
	LEAF::LeafNode(Iter begin, Iter end, shared_ptr<LessThan> funcPtr)
		: Base_CRTP(begin, end, funcPtr)
	{ }

	NODE_TEMPLATE
	LeafNode<Key, Value, BtreeOrder>::LeafNode(const LeafNode& that, LeafNode* previous, LeafNode* next)
		: Base_CRTP(that), _previous(previous), _next(next)
	{ }

	NODE_TEMPLATE
	LEAF::LeafNode(LeafNode&& that) noexcept
		: Base_CRTP(std::move(that)), _next(that._next)
	{ }

	NODE_TEMPLATE
	const Value&
	LEAF::operator[](const Key& key)
	{
		auto& e = this->elements_[key];
		return Base::Ele::value_Ref(e);
	}

	NODE_TEMPLATE
	pair<Key, Value>
	LEAF::operator[](uint16_t i)
	{
		auto& e = Base::elements_[i];
		return make_pair(e.first, Base::Ele::value_Ref(e.second));
	}

	NODE_TEMPLATE
	LEAF*
	LEAF::nextLeaf() const
	{
		return _next;
	}

	NODE_TEMPLATE
	void
	LEAF::nextLeaf(LeafNode* next)
	{
		_next = next;
	}

	NODE_TEMPLATE
	void
	LEAF::previousLeaf(LeafNode* previous)
	{
		_previous = previous;
	}

	NODE_TEMPLATE
	LEAF*
	LEAF::previousLeaf() const
	{
		return _previous;
	}

#undef LEAF
#undef NODE_TEMPLATE
}

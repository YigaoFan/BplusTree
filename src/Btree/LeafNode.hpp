#pragma once
#include <utility>
#include "Basic.hpp"
#include "NodeBaseCrtp.hpp"

namespace Collections
{
#define LEAF LeafNode<Key, Value, BtreeOrder>

	template <typename Key, typename Value, order_int BtreeOrder>
	class LeafNode : public NodeBase_CRTP<LEAF, Key, Value, BtreeOrder>
	{
	private:
		using Base = NodeBase<Key, Value, BtreeOrder>;
		using Base_CRTP = NodeBase_CRTP<LEAF, Key, Value, BtreeOrder>;
		using typename Base::LessThan;

	public:
		template <typename Iter>
		LeafNode(Iter begin, Iter end, shared_ptr<LessThan> lessThan)
			: Base_CRTP(begin, end, lessThan)
		{}

		LeafNode(const LeafNode& that, LeafNode* previous = nullptr, LeafNode* next = nullptr)
			: Base_CRTP(that), _previous(previous), _next(next)
		{}

		LeafNode(LeafNode&& that) noexcept
			: Base_CRTP(std::move(that)), _next(that._next)
		{}

		~LeafNode() override = default;

		const Value& operator[](const Key& key)
		{
			auto& e = this->elements_[key];
			return Base::Ele::value_Ref(e);
		}

		pair<Key, Value> operator[](uint16_t i)
		{
			auto& e = Base::elements_[i];
			return make_pair(e.first, Base::Ele::value_Ref(e.second));
		}

		LeafNode* nextLeaf() const
		{
			return _next;
		}

		void nextLeaf(LeafNode* next)
		{
			_next = next;
		}

		LeafNode* previousLeaf() const
		{
			return _previous;
		}

		void  previousLeaf(LeafNode* previous)
		{
			_previous = previous;
		}

	private:
		LeafNode* _next{ nullptr };
		LeafNode* _previous{ nullptr };
	};
#undef LEAF
}

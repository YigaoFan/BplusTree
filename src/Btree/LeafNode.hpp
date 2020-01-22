#pragma once
#include <utility>
#include "Basic.hpp"
#include "Enumerator.hpp"
#include "Elements.hpp"
#include "NodeBase.hpp"
#include "NodeBaseCrtp.hpp"

namespace Collections
{
	using ::std::move;
#define LEAF LeafNode<Key, Value, BtreeOrder>

	template <typename Key, typename Value, order_int BtreeOrder>
	class LeafNode : public NodeBase_CRTP<LEAF, Key, Value, BtreeOrder>
	{
	private:
		using Base = NodeBase<Key, Value, BtreeOrder>;
		using Base_CRTP = NodeBase_CRTP<LEAF, Key, Value, BtreeOrder>;
		using typename Base::LessThan;
		Elements<Key, Value, BtreeOrder> _elements;
		LeafNode* _next{ nullptr };
		LeafNode* _previous{ nullptr };

	public:
		template <typename Iter>
		LeafNode(Iter begin, Iter end, shared_ptr<LessThan> lessThan)
			: Base_CRTP(begin, end, lessThan)
		{}

		template <typename Iterator>
		LeafNode(Enumerator<pair<Key, Value>, Iterator> enumerator, shared_ptr<LessThan> lessThan)
			: Base_CRTP(enumerator, lessThan)
		{}

		LeafNode(LeafNode const& that, LeafNode* previous = nullptr, LeafNode* next = nullptr)
			: Base_CRTP(that), _previous(previous), _next(next)
		{}

		LeafNode(LeafNode&& that) noexcept
			: Base_CRTP(std::move(that)), _next(that._next)
		{}

		~LeafNode() override = default;

		vector<Key> Keys() const override
		{
			//return this->elements_.Keys();// TODO why can not use elements directly?
			// I test struct can use this.
			return CollectKeys(move(vector<Key>{}));
		}

		bool Middle() const override
		{
			return false;
		}

		Value const& operator[](Key const& key)
		{
			return this->elements_[key];
		}

		pair<Key, Value> const& operator[](uint16_t i)
		{
			return this->elements_[i];
		}

		LeafNode* NextLeaf() const
		{
			return _next;
		}

		void NextLeaf(LeafNode* next)
		{
			_next = next;
		}

		LeafNode* PreviousLeaf() const
		{
			return _previous;
		}

		void PreviousLeaf(LeafNode* previous)
		{
			_previous = previous;
		}

	private:
		vector<Key> CollectKeys(vector<Key> previousNodesKeys)
		{
			auto&& ks = this->elements_->Keys();
			previousNodesKeys.insert(previousNodesKeys.end(), ks.begin(), ks.end());
			if (_next == nullptr)
			{
				return move(previousNodesKeys);
			}

			return _next->CollectKeys(move(previousNodesKeys));
		}
	};
#undef LEAF
}

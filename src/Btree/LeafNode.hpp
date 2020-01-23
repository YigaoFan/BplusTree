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
		using Ele = Elements<Key, Value, BtreeOrder>;
		using typename Ele::LessThan;
		using Base_CRTP = NodeBase_CRTP<LEAF, Key, Value, BtreeOrder>;
		Elements<Key, Value, BtreeOrder> _elements;
		LeafNode* _next{ nullptr };
		LeafNode* _previous{ nullptr };

	public:
		template <typename Iterator>
		LeafNode(Enumerator<pair<Key, Value>, Iterator> enumerator, shared_ptr<LessThan> lessThan)
			: Base_CRTP(), _elements(enumerator, lessThan)
		{}

		LeafNode(LeafNode const& that, LeafNode* previous = nullptr, LeafNode* next = nullptr)
			: Base_CRTP(that), _elements(that._elements), _next(next), _previous(previous)
		{}

		LeafNode(LeafNode&& that) noexcept
			: Base_CRTP(move(that)), _elements(move(that._elements)),
			 _next(that._next), _previous(that._previous)
		{}

		~LeafNode() override = default;

		bool Middle() const override
		{
			return false;
		}

		vector<Key> Keys() const override
		{
			//return this->elements_.Keys();// TODO why can not use elements directly?
			// I test struct can use this.
			return CollectKeys(move(vector<Key>{}));
		}

		Key const& MinKey() override const
		{
			return _elements[0].first;
		}

		void Add(pair<Key, Value> p) override
		{
			if (!_elements.Full())
			{
				return _elements.Add(move(p));
			}

			// Compare and choose previous and next LeafNode
		}

		virtual void Remove(Key const& key) override
		{
			return _elements.Remove(key);
		}

		Value const& operator[](Key const& key)
		{
			return _elements[key];
		}

		pair<Key, Value> const& operator[](order_int i)
		{
			return _elements[i];
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
			auto&& ks = _elements->Keys();
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

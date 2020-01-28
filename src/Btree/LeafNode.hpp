#pragma once
#include <utility>
#include "Basic.hpp"
#include "Enumerator.hpp"
#include "Elements.hpp"
#include "NodeBase.hpp"
#include "NodeAddRemoveCommon.hpp"

namespace Collections
{
	using ::std::move;
	using ::std::make_unique;
	using ::std::remove_pointer_t;

#define LEAF LeafNode<Key, Value, BtreeOrder>
	template <typename Key, typename Value, order_int BtreeOrder>
	class LeafNode : public NodeBase<Key, Value, BtreeOrder>
#undef LEAF
	{
	private:
		using _LessThan = LessThan<Key>;
		using Base = NodeBase<Key, Value, BtreeOrder>;
		Elements<Key, Value, BtreeOrder, _LessThan> _elements;
		LeafNode* _next{ nullptr };
		LeafNode* _previous{ nullptr };

	public:
		LeafNode(shared_ptr<_LessThan> lessThan)
			: Base(), _elements(/*right value passed to ref value EmptyEnumerator<pair<Key, Value>>(),*/ lessThan)
		{ }

		template <typename Iterator>
		LeafNode(Enumerator<pair<Key, Value>&, Iterator> enumerator, shared_ptr<_LessThan> lessThan)
			: Base(), _elements(enumerator, lessThan)
		{}

		LeafNode(LeafNode& that, LeafNode* previous = nullptr, LeafNode* next = nullptr)
			: Base(that), _elements(that._elements), _next(next), _previous(previous)
		{}

		LeafNode(LeafNode&& that) noexcept
			: Base(move(that)), _elements(move(that._elements)),
			 _next(that._next), _previous(that._previous)
		{}

		~LeafNode() override = default;

		unique_ptr<Base> Clone() override
		{
			return make_unique<LeafNode>(*this);
		}

		bool Middle() const override
		{
			return false;
		}

		vector<Key> Keys() const override
		{
			//return this->elements_.Keys();// TODO why can not use elements directly?
			// I test struct can use this.
			return CollectKeys();
		}

		Key const& MinKey() const override
		{
			return _elements[0].first;
		}

		bool ContainsKey(Key const& key) const override
		{
			return _elements.ContainsKey(key);
		}

		Value GetValue(Key const& key) const override
		{
			return _elements[key];
		}

		void ModifyValue(Key const& key, Value value) override
		{
			_elements[key] = move(value);
		}

		void Add(pair<Key, Value> p) override
		{
			if (!_elements.Full())
			{
				_elements.Add(move(p));
				return;
			}

			ADD_COMMON(true);
		}

		virtual void Remove(Key const& key) override
		{
			_elements.RemoveKey(key);
			REMOVE_COMMON;
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
		vector<Key> CollectKeys(vector<Key> previousNodesKeys = {}) const
		{
			auto&& ks = _elements.Keys();
			previousNodesKeys.insert(previousNodesKeys.end(), ks.begin(), ks.end());
			if (_next == nullptr)
			{
				return move(previousNodesKeys);
			}

			return _next->CollectKeys(move(previousNodesKeys));
		}
	};
}
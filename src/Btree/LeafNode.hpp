#pragma once
#include <utility>
#include <iterator>
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
	using ::std::back_inserter;

	template <typename Key, typename Value, order_int BtreeOrder>
	class LeafNode : public NodeBase<Key, Value, BtreeOrder>
	{
	private:
		using _LessThan = LessThan<Key>;
		using Base = NodeBase<Key, Value, BtreeOrder>;
		Elements<Key, Value, BtreeOrder, _LessThan> _elements;
		LeafNode* _next{ nullptr };
		LeafNode* _previous{ nullptr };
	public:
		bool Middle() const override { return false; }

		LeafNode(shared_ptr<_LessThan> lessThan)
			: Base(), _elements(lessThan)
		{ }

		template <typename Iterator>
		LeafNode(Enumerator<pair<Key, Value>&, Iterator> enumerator, shared_ptr<_LessThan> lessThan)
			: Base(), _elements(enumerator, lessThan)
		{ }

		LeafNode(LeafNode const& that) : Base(that), _elements(that._elements)
		{ }

		LeafNode(LeafNode&& that) noexcept
			: Base(move(that)), _elements(move(that._elements)),
			 _next(that._next), _previous(that._previous)
		{ }

		~LeafNode() override = default;

		unique_ptr<Base> Clone() const override { return make_unique<LeafNode>(*this); }

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
			return _elements.GetValue(key);
		}

		void ModifyValue(Key const& key, Value value) override
		{
			_elements.GetValue(key) = move(value);
		}

		void Add(pair<Key, Value> p) override
		{
			if (!_elements.Full())
			{
				return _elements.Add(move(p), [this]() 
				{
					(*this->_minKeyChangeCallbackPtr)(this->MinKey(), this);
				});
			}

			ADD_COMMON(true);
		}

		void Remove(Key const& key) override
		{
			auto i = _elements.IndexKeyOf(key);
			_elements.RemoveAt(i);
			if (i == 0)
			{
				(*this->_minKeyChangeCallbackPtr)(MinKey(), this);
			}

			AFTER_REMOVE_COMMON(true);
			// LeafNode no need to handle NoWhereToProcess,
			// Cannot put code here
		}

		LeafNode* Next() const { return _next; }
		LeafNode* Previous() const { return _previous; }
		void Next(LeafNode* next) { _next = next; }
		void Previous(LeafNode* previous) { _previous = previous; }
	private:
		vector<Key> CollectKeys(vector<Key> previousNodesKeys = {}) const
		{
			auto ks = _elements.Keys();
			previousNodesKeys.reserve(previousNodesKeys.size() + ks.size());
			move(ks.begin(), ks.end(), back_inserter(previousNodesKeys));
			return this->Next() == nullptr ? 
				move(previousNodesKeys) : this->Next()->CollectKeys(move(previousNodesKeys));
		}
	};
}
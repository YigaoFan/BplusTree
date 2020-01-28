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

			bool addToPre = false, addToNxt = false;
			if (_previous == nullptr)
			{
				if (_next != nullptr)
				{
					addToNxt = true;
				}
			}
			else
			{
				if (_next == nullptr)
				{
					addToPre = true;
				}
				else
				{
					switch (Base::ChooseOperatePosition<Operation::Add>(_previous->_elements.Count(),
																  this->_elements.Count(),
																  _next->_elements.Count()))
					{
					case Position::Previous:
						addToPre = true;
						break;
					case Position::Next:
						addToNxt = true;
						break;
					}
				}
			}

			if (addToPre)
			{
				if (!_previous->_elements.Full())
				{
					_previous->_elements.Append(_elements.ExchangeMin(move(p)));
					return;
				}
			}
			else if (addToNxt)
			{
				if (!_next->_elements.Full())
				{
					_next->_elements.Insert(_elements.ExchangeMax(move(p)));
					return;
				}
			}
			
			// Create new empty LeafNode
			auto newNxtLeaf = make_unique<LeafNode>(_elements.LessThanPtr);
			newNxtLeaf->_next = this->_next;
			newNxtLeaf->_previous = this;
			this->_next = newNxtLeaf.get();

			// Add
			auto i = _elements.SuitPosition<true>(p.first);
			constexpr auto middle = (BtreeOrder % 2) ? (BtreeOrder / 2 + 1) : (BtreeOrder / 2);
			if (i <= middle)
			{
				auto items = this->_elements.PopOutItems(middle);
				this->_elements.Add(move(p));
				newNxtLeaf->_elements.Add(move(items));
			}
			else
			{
				auto items = this->_elements.PopOutItems(BtreeOrder - middle);
				newNxtLeaf->_elements.Add(move(items));
				newNxtLeaf->_elements.Add(move(p));
			}

			// TODO replace this with key
			this->_upNodeAddSubNodeCallback(this, move(newNxtLeaf));
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
#undef REMOVE_COMMON

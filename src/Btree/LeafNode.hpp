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
	using ::std::make_unique;

#define LEAF LeafNode<Key, Value, BtreeOrder>
	template <typename Key, typename Value, order_int BtreeOrder>
	class LeafNode : public NodeBase_CRTP<LEAF, Key, Value, BtreeOrder>
	{
	private:
		using _LessThan = LessThan<Key>;
		using Base_CRTP = NodeBase_CRTP<LEAF, Key, Value, BtreeOrder>;
		Elements<Key, Value, BtreeOrder> _elements;
		LeafNode* _next{ nullptr };
		LeafNode* _previous{ nullptr };

#undef LEAF
	public:
		LeafNode(shared_ptr<_LessThan> lessThan)
			: Base_CRTP(), _elements(/*right value passed to ref value EmptyEnumerator<pair<Key, Value>>(),*/ lessThan)
		{ }

		template <typename Iterator>
		LeafNode(Enumerator<pair<Key, Value>, Iterator> enumerator, shared_ptr<_LessThan> lessThan)
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
					auto preCount = _previous->_elements.Count();
					auto nxtCount = _next->_elements.Count();
					auto average = (preCount + this->_elements.Count() + nxtCount) / 3;
					if (int(preCount - average) < int(nxtCount - average))
					{
						addToPre = true;
					}
					else
					{
						addToNxt = true;
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

			this->_upNodeAddSubNodeCallback(this, move(newNxtLeaf));
		}

		virtual void Remove(Key const& key) override
		{
			_elements.Remove(key);
			constexpr order_int lowBound = 1 + ((BtreeOrder - 1) / 2);
			if (_elements.Count() < lowBound)
			{
				if (_next != nullptr)
				{
					if (_next->_elements.Count() == lowBound)
					{
						// Combine
						// Use "this" to emphasize the operated object
						auto items = this->_elements.PopOutItems(this->_elements.Count());
						_next->_elements.Add(move(items));
						// Delete this
						this->_upNodeDeleteSubNodeCallback(this);
					}
					else
					{
						// Means next _elements bigger than lowBound
						// only one which is lower than lowBound is root leaf which doesn't have siblings
						// Or steal one, could think steal one from which sibling in more balance view
						auto item = this->_next->_elements.FrontPopOut();
						this->_elements.Append(move(item));
					}
				}
				else if (_previous != nullptr)
				{
					if (_previous->_elements.Count() == lowBound)
					{
						auto items = this->_elements.PopOutItems(this->_elements.Count());
						_previous->_elements.Add(move(items));
						//_previous->_elements.Add(CreateEnumerator(items./*r*/begin(), items./*r*/end())); why error
						this->_upNodeDeleteSubNodeCallback(this);
					}
					else
					{
						// Means next _elements bigger than lowBound
						// Or steal one, could think steal one from which sibling in more balance view
						auto items = this->_previous->_elements.PopOutItems(1);
						this->_elements.Insert(move(items[0]));
					}
				}
			}
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

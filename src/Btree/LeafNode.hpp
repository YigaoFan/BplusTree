#pragma once
#include <utility>
#include <cmath>
#include "Basic.hpp"
#include "Enumerator.hpp"
#include "Elements.hpp"
#include "NodeBase.hpp"
#include "NodeBaseCrtp.hpp"

namespace Collections
{
	using ::std::move;
	using ::std::abs;
	using ::std::make_unique;

#define LEAF LeafNode<Key, Value, BtreeOrder>

	template <typename Key, typename Value, order_int BtreeOrder>
	class LeafNode : public NodeBase_CRTP<LEAF, Key, Value, BtreeOrder>
#undef LEAF
	{
	private:
		using Ele = Elements<Key, Value, BtreeOrder>;
		using typename Ele::LessThan;
		using Base_CRTP = NodeBase_CRTP<LEAF, Key, Value, BtreeOrder>;
		Elements<Key, Value, BtreeOrder> _elements;
		LeafNode* _next{ nullptr };
		LeafNode* _previous{ nullptr };
		// TODO wait to init
		function<void(LeafNode*, LeafNode*)> _upNodeCallback;

	public:
		LeafNode(shared_ptr<LessThan> lessThan)
			: Base_CRTP(), _elements(EmptyEnumerator<pair<Key, Value>>(), lessThan)
		{ }

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
			return CollectKeys();
		}

		Key const& MinKey() override const
		{
			return _elements[0].first;
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
					if (abs(preCount - average) > abs(nxtCount - average))
					{
						addToNxt = true;
					}
					else
					{
						addToPre = true;
					}
				}
			}

			if (addToPre)
			{
				if (!_previous._elements.Full())
				{
					_previous->_elements.Append(_elements.ExchangeMin(move(p)));
					return;
				}
			}
			else if (addToNxt)
			{
				if (!_next._elements.Full())
				{
					_next->_elements.Insert(_elements.ExchangeMax(move(p)));
					return;
				}
			}
			
			// Create new empty LeafNode
			auto newNxtLeaf = make_unique<LeafNode>(_elements.LessThanPtr);
			newNxtLeaf._next = this->_next;
			newNxtLeaf._previous = this;
			this->_next = newNxtLeaf.get();

			auto doBalance = [&](order_int preNodeRemoveCount)
			{
				this->_elements.PopOutItems(removeCount);
			};
			// Add
			auto i = _elements.SuitPosition(p.first);
			constexpr auto middle = (BtreeOrder % 2) ? (BtreeOrder / 2 + 1) : (BtreeOrder / 2);
			if (i <= middle)
			{
				auto items = this->_elements.PopOutItems(middle);
				this->_elements.Add(move(p));
				newNxtLeaf._elements.Add(CreateEnumerator(items.rbegin(), items.rend()));
			}
			else
			{
				auto items = this->_elements.PopOutItems(BtreeOrder - middle);
				newNxtLeaf._elements.Add(CreateEnumerator(items.rbegin(), items.rend()));
				newNxtLeaf->_elements.Add(move(p));
			}

			_upNodeCallback(this, newNxtLeaf);
		}

		virtual void Remove(Key const& key) override
		{
			_elements.Remove(key);
			constexpr auto lowBound = 1 + ((BtreeOrder - 1) / 2);
			if (_elements.Count() < lowBound)
			{
				auto newLeaf = make_unique
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
		vector<Key> CollectKeys(vector<Key> previousNodesKeys = {})
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
}

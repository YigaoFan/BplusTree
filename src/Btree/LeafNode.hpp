#pragma once
#include <utility>
#include "Basic.hpp"
#include "Enumerator.hpp"
#include "Elements.hpp"
#include "NodeBase.hpp"

namespace Collections
{
	using ::std::move;
	using ::std::make_unique;

	namespace
	{
		enum Position
		{
			Previous,
			Next,
		};

		// Why this enum can not define in LeafNode class? because it has LeafNode::Add adn LeafNode::Remove
		enum Operation
		{
			Add,
			Remove,
		};
	}

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
		LeafNode(Enumerator<pair<Key, Value>, Iterator> enumerator, shared_ptr<_LessThan> lessThan)
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
					switch (ChooseOperatePosition<Operation::Add>(_previous->_elements.Count(),
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
			constexpr lowBound = LowBound;
			if (_elements.Count() < lowBound)
			{
				bool nxtStealable = false, preStealable = false;
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
						return;
					}
					else
					{
						// Means next _elements bigger than lowBound
						// only one which is lower than lowBound is root leaf which doesn't have siblings
						// Or steal one, could think steal one from which sibling in more balance view
						nxtStealable = true;
					}
				}
				
				if (_previous != nullptr)
				{
					if (_previous->_elements.Count() == lowBound)
					{
						auto items = this->_elements.PopOutItems(this->_elements.Count());
						_previous->_elements.Add(move(items));
						//_previous->_elements.Add(CreateEnumerator(items./*r*/begin(), items./*r*/end())); why error
						// TODO replace this with key
						this->_upNodeDeleteSubNodeCallback(this);
						return;
					}
					else
					{
						preStealable = true;
					}
				}

				if (nxtStealable && preStealable)
				{
					switch (ChooseOperatePosition<Operation::Remove>(_previous->_elements.Count(), this->_elements.Count(),
						_next->_elements.Count()))
					{
					case Position::Next:
						goto StealNxt;
					case Position::Previous:
						goto StealPre;
					}
				}
				else if (nxtStealable)
				{
					goto StealNxt;

				}
				else if (preStealable)
				{
					goto StealPre;
				}
				else
				{
					goto NothingToDo;
				}

			StealNxt:
				{
					auto item = this->_next->_elements.FrontPopOut();
					this->_elements.Append(move(item));
					return;
				}
			StealPre:
				{
					auto items = this->_previous->_elements.PopOutItems(1);
					this->_elements.Insert(move(items[0]));
					return;
				}
			NothingToDo:
				return;
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


		template <Operation Op>
		static Position ChooseOperatePosition(order_int preCount, order_int thisCount, order_int nxtCount)
		{
			if constexpr (Op == Operation::Add)
			{
				auto average = (preCount + thisCount + nxtCount) / 3;
				if (int(preCount - average) < int(nxtCount - average))
				{
					return Position::Previous;
				}
				else
				{
					return Position::Next;
				}
			}
			else
			{
				return ChooseOperatePosition<Operation::Add>(preCount, thisCount, nxtCount) == Position::Previous ?
					Position::Next : Position::Previous;
			}
		}
	};
}

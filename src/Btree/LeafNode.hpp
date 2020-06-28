#pragma once
#include <utility>
#include <iterator>
#include "../Basic/Exception.hpp"
#include "Basic.hpp"
#include "Enumerator.hpp"
#include "Elements.hpp"
#include "NodeBase.hpp"
#include "NodeAddRemoveCommon.hpp"
#include "../FuncLib/FriendFuncLibDeclare.hpp"

namespace Collections
{
	using ::std::move;
	using ::std::make_unique;
	using ::std::unique_ptr;
	using ::std::remove_pointer_t;
	using ::std::back_inserter;
	using ::Basic::NotImplementException;

	template <typename Key, typename Value, order_int BtreeOrder, template <typename...> class Ptr = unique_ptr>
	class LeafNode : public NodeBase<Key, Value, BtreeOrder, Ptr>
	{
	private:
		friend struct FuncLib::ByteConverter<LeafNode, false>;
		friend struct FuncLib::TypeConverter<LeafNode, false>;
		using _LessThan = LessThan<Key>;
		using Base = NodeBase<Key, Value, BtreeOrder, Ptr>;
		Elements<Key, Value, BtreeOrder, _LessThan> _elements;// Key, Value type should change
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

		Ptr<Base> Clone() const override { return make_unique<LeafNode>(*this); }

		vector<Key> Keys() const override
		{
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

			auto next = _next;
			auto previous = _previous;
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

			constexpr auto lowBound = Base::LowBound;
			if (_elements.Count() < lowBound)
			{
				auto next = _next;
				auto previous = _previous;
				AFTER_REMOVE_COMMON(true);
				// LeafNode no need to handle NoWhereToProcess,
				// Cannot put code here
			}
		}

		bool MoveDown(Base*&) const override
		{
			throw NotImplementException();
		}

		bool MoveLeft(Base*&) const override
		{
			throw NotImplementException();
		}

		bool MoveRight(Base*&) const override
		{
			throw NotImplementException();
		}

		vector<Key> SubNodeMinKeys() const override
		{
			return _elements.Keys();
		}

		vector<Base*> SubNodes() const override
		{
			return {};
		}

		LeafNode* Next() const { return _next; }
		LeafNode* Previous() const { return _previous; }
		void Next(LeafNode* next) { _next = next; }
		void Previous(LeafNode* previous) { _previous = previous; }
	private:
		LeafNode(decltype(_elements) elements) : Base(), _elements(move(elements))// TODO less than?
		{
			// TODO
		}

		vector<Key> CollectKeys(vector<Key> previousNodesKeys = {}) const
		{
			auto ks = _elements.Keys();
			previousNodesKeys.reserve(previousNodesKeys.size() + ks.size());
			move(ks.begin(), ks.end(), back_inserter(previousNodesKeys));
			return this->Next() == nullptr ? 
				move(previousNodesKeys) : this->Next()->CollectKeys(move(previousNodesKeys));
		}

		/// previous is not null
		void SetRelationWhileCombineToPrevious(LeafNode* previous)
		{
			previous->_next = this->_next;

			if (this->_next != nullptr)
			{
				this->_next->_previous = previous;
			}
		}

		/// next is not null
		void SetRelationWhileCombineNext(LeafNode* next)
		{
			this->_next = next->_next;

			if (next->_next != nullptr)
			{
				next->_next->_previous = this;
			}
		}

		void SetRelationWhileSplitNewNext(LeafNode* newNext)
		{
			newNext->_next = this->_next;
			newNext->_previous = this;
			if (this->_next != nullptr)
			{
				this->_next->_previous = newNext;
			}

			this->_next = newNext;
		}

		// Below methods for same node internal use
		void AppendItems(vector<typename decltype(_elements)::Item> items)
		{
			for (auto& i : items)
			{
				Append(move(i));
			}
		}

		void Append(typename decltype(_elements)::Item item)
		{
			_elements.Append(move(item));
		}

		void EmplaceHead(typename decltype(_elements)::Item item)
		{
			_elements.EmplaceHead(move(item));
			(*this->_minKeyChangeCallbackPtr)(this->MinKey(), this);
		}

		void ProcessedAdd(typename decltype(_elements)::Item item)
		{
			_elements.Add(move(item), [this]()
			{
				(*this->_minKeyChangeCallbackPtr)(this->MinKey(), this);
			});
		}

		typename decltype(_elements)::Item
		ExchangeMin(typename decltype(_elements)::Item item)
		{
			auto min = _elements.ExchangeMin(move(item));
			(*this->_minKeyChangeCallbackPtr)(this->MinKey(), this);
			return move(min);
		}

		typename decltype(_elements)::Item
		ExchangeMax(typename decltype(_elements)::Item item)
		{
			return _elements.ExchangeMax(move(item));
		}
	};
}
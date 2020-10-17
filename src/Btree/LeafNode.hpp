#pragma once
#include <utility>
#include <iterator>
#include <type_traits>
#include "../Basic/Exception.hpp"
#include "Basic.hpp"
#include "Enumerator.hpp"
#include "Elements.hpp"
#include "NodeBase.hpp"
#include "NodeAddRemoveCommon.hpp"

namespace Collections
{
	using ::FuncLib::Switch;
	using ::FuncLib::TakeWithDiskPos;
	using ::std::back_inserter;
	using ::std::is_same_v;
	using ::std::make_shared;
	using ::std::make_unique;
	using ::std::move;
	using ::std::remove_const_t;
	using ::std::remove_pointer_t;
	using ::std::result_of_t;
	using ::std::unique_ptr;

	// 这里用指针参数不太好，可以用个枚举类型。因为这个指针参数不是所有指针都用，有的要用 OwnerLess 的指针
	template <typename Key, typename Value, order_int BtreeOrder, template <typename...> class Ptr = unique_ptr>
	class LeafNode : public NodeBase<Key, Value, BtreeOrder, Ptr>, 
					 public TakeWithDiskPos<LeafNode<Key, Value, BtreeOrder, Ptr>, IsDisk<Ptr> ? Switch::Enable : Switch::Disable> 
	{
	private:
		friend struct FuncLib::ByteConverter<LeafNode, false>;
		friend struct FuncLib::TypeConverter<LeafNode<Key, Value, BtreeOrder, unique_ptr>>;
		using _LessThan = LessThan<Key>;
		using Base = NodeBase<Key, Value, BtreeOrder, Ptr>;
#define RAW_PTR(TYPE) typename Base::template OwnerLessPtr<TYPE>
		using StoredKey = typename TypeSelector<GetStorePlace<Ptr>, Refable::No, Key>::Result;
		using StoredValue = typename TypeSelector<GetStorePlace<Ptr>, Refable::No, Value>::Result;
		Elements<StoredKey, StoredValue, BtreeOrder, _LessThan> _elements;
		typename Base::template OwnerLessPtr<LeafNode> _next{nullptr};
		typename Base::template OwnerLessPtr<LeafNode> _previous{nullptr};

	public:
		bool Middle() const override { return false; }

		LeafNode(shared_ptr<_LessThan> lessThan) : Base(), _elements(lessThan)
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

		Ptr<Base> Clone() const override 
		{
			return CopyNode(this);
		}

		DEF_LESS_THAN_SETTER

		DEF_COPY_NODE

		DEF_NEW_EMPTY_NODE

		vector<Key> Keys() const override
		{
			return CollectKeys();
		}

		result_of_t<decltype(&Base::MinKey)(Base)> const MinKey() const override
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

		vector<Key> SubNodeMinKeys() const override
		{
			if constexpr (is_same_v<StoredKey, Key>)
			{
				return _elements.Keys();
			}
			else
			{
				auto storedKeys = _elements.Keys();
				vector<Key> ks;
				ks.reserve(_elements.Count());
				for (auto& x : storedKeys)
				{
					ks.push_back(x);
				}

				return ks;
			}
		}

		result_of_t<decltype (&Base::SubNodes)(Base)> SubNodes() const override
		{
			return {};
		}

		decltype(_next)     Next()     const { return _next; }
		decltype(_previous) Previous() const { return _previous; }
		void Next(decltype(_next) next)             { _next = move(next); }
		void Previous(decltype(_previous) previous) { _previous = move(previous); }
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
		void SetRelationWhileCombineToPrevious(RAW_PTR(LeafNode) previous)
		{
			previous->_next = this->_next;

			if (this->_next != nullptr)
			{
				this->_next->_previous = previous;
			}
		}

		/// next is not null
		void SetRelationWhileCombineNext(RAW_PTR(LeafNode) next)
		{
			this->_next = next->_next;

			if (next->_next != nullptr)
			{
				next->_next->_previous = GetRawPtr(this);
			}
		}

		void SetRelationWhileSplitNewNext(RAW_PTR(LeafNode) newNext)
		{
			newNext->_next = this->_next;
			newNext->_previous = GetRawPtr(this);
			if (this->_next != nullptr)
			{
				this->_next->_previous = newNext;
			}

			this->_next = newNext;
		}

		// TODO 再想一想引用这里，和上面几个函数引用的地方类型对不对
		template <typename T>
		static RAW_PTR(LeafNode) GetRawPtr(T ptr)// 需要 T 至少和 LeafNode 有关，这里传进来的都是 pointer 类型
		{
			using Ret = RAW_PTR(LeafNode);
			if constexpr (is_same_v<T, Ret>)
			{
				return ptr;
			}
			else if (std::is_pointer_v<T>)
			{
				return ptr->GetOwnerLessDiskPtr();
			}
			else
			{
				// not support
			}
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
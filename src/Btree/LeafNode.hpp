#pragma once
#include <utility>
#include <iterator>
#include <exception>
#include <type_traits>
#include "../Basic/Exception.hpp"
#include "Basic.hpp"
#include "Enumerator.hpp"
#include "Elements.hpp"
#include "NodeBase.hpp"

namespace Collections
{
	using Basic::FuncTraits;
	using Basic::GetMemberFuncType;
	using ::FuncLib::Persistence::Switch;
	using ::FuncLib::Persistence::TakeWithDiskPos;
	using ::std::back_inserter;
	using ::std::is_same_v;
	using ::std::make_unique;
	using ::std::move;
	using ::std::out_of_range;
	using ::std::result_of_t;

	template <typename Key, typename Value, order_int BtreeOrder, StorePlace Place = StorePlace::Memory>
	class LeafNode : public NodeBase<Key, Value, BtreeOrder, Place>,
					 public TakeWithDiskPos<LeafNode<Key, Value, BtreeOrder, Place>, IsDisk<Place> ? Switch::Enable : Switch::Disable>
	{
	private:
		template <typename... Ts>
		using Ptr = typename TypeConfig::template Ptr<Place>::template Type<Ts...>;
		friend struct FuncLib::Persistence::ByteConverter<LeafNode, false>;
		friend struct FuncLib::Persistence::TypeConverter<LeafNode<Key, Value, BtreeOrder, StorePlace::Memory>>;
		using Base1 = NodeBase<Key, Value, BtreeOrder, Place>;
		using Base2 = TakeWithDiskPos<LeafNode<Key, Value, BtreeOrder, Place>, IsDisk<Place> ? Switch::Enable : Switch::Disable>;
#define RAW_PTR(TYPE) typename Base1::template OwnerLessPtr<TYPE>
		template <bool IsLeaf, typename Node, typename Item>
		friend void Base1::AddWith(RAW_PTR(Node) previous, RAW_PTR(Node) next, Node* self, Item p);
		template <bool IsLeaf, typename Node, typename NoWhereToProcessCallback>
		friend void Base1::AdjustAfterRemove(RAW_PTR(Node) previous, RAW_PTR(Node) next, Node* self, NoWhereToProcessCallback noWhereToProcessCallback);

		using StoredKey = typename Base1::StoredKey;
		using StoredValue = typename Base1::StoredValue;
		Elements<StoredKey, StoredValue, BtreeOrder, LessThan<Key>> _elements;
		RAW_PTR(LeafNode) _next{nullptr};
		RAW_PTR(LeafNode) _previous{nullptr};

	public:
		bool Middle() const override { return false; }

		LeafNode() : Base1(), _elements(Base1::_lessThan)
		{ }

		LeafNode(IEnumerator<pair<StoredKey, StoredValue>> auto enumerator)
			: Base1(), _elements(enumerator, Base1::_lessThan)
		{ }

		LeafNode(LeafNode const& that) : Base1(that), Base2(that), _elements(that._elements)
		{ }

		LeafNode(LeafNode&& that) noexcept
			: Base1(move(that)), Base2(move(that)),
			  _elements(move(that._elements)),
			  _next(that._next), _previous(that._previous)
		{ }

		Ptr<Base1> Clone() const
		{
			return this->CopyNode(this);
		}

		vector<Key> LetMinLeafCollectKeys() const override
		{
			return CollectKeys();
		}

		result_of_t<decltype(&Base1::MinKey)(Base1)> MinKey() const override
		{
			return _elements[0].first;
		}

#define ARG_TYPE_IN_BASE(METHOD, IDX) typename FuncTraits<typename GetMemberFuncType<decltype(&Base1::METHOD)>::Result>::template Arg<IDX>::Type
		bool ContainsKey(ARG_TYPE_IN_BASE(ContainsKey, 0) key) const override
		{
			return _elements.ContainsKey(move(key));
		}

		StoredValue& GetValue(ARG_TYPE_IN_BASE(GetValue, 0) key) override
		{
			return _elements.GetValue(move(key));
		}

		void ModifyValue(ARG_TYPE_IN_BASE(ModifyValue, 0) key, ARG_TYPE_IN_BASE(ModifyValue, 1) value) override
		{
			_elements.GetValue(move(key)) = move(value);
		}

		void Add(ARG_TYPE_IN_BASE(Add, 0) p) override
		{
			if (not _elements.Full())
			{
				return _elements.Add(move(p), [this]() 
				{
					(*this->_minKeyChangeCallbackPtr)(this->MinKey(), this);
				});
			}

			Base1::template AddWith<true>(_previous, _next, this, move(p));
		}

		void Remove(ARG_TYPE_IN_BASE(Remove, 0) key) override
		{
			auto i = _elements.IndexKeyOf(move(key));
			_elements.RemoveAt(i);
			if (i == 0)
			{
				(*this->_minKeyChangeCallbackPtr)(MinKey(), this);
			}

			constexpr auto lowBound = Base1::LowBound;
			if (_elements.Count() < lowBound)
			{
				Base1::template AdjustAfterRemove<true>(_previous, _next, this, []{});
			}
		}
#undef ARG_TYPE_IN_BASE

		vector<Key> KeysInThisNode() const override
		{
			return this->GetKeysFrom(_elements);
		}

		result_of_t<decltype (&Base1::SubNodes)(Base1)> SubNodes() const override
		{
			return {};
		}

		RecursiveGenerator<pair<StoredKey, StoredValue>*> GetStoredPairEnumerator() override
		{
			for (auto& e : _elements)
			{
				co_yield &e;
			}
		}

		decltype(_next)     Next()     const { return _next; }
		decltype(_previous) Previous() const { return _previous; }
		void Next(decltype(_next) next)             { _next = move(next); }
		void Previous(decltype(_previous) previous) { _previous = move(previous); }
	private:
		// element LessThanPtr is not set
		LeafNode(decltype(_elements) elements, 
			RAW_PTR(LeafNode) previous, RAW_PTR(LeafNode) next)
		 : Base1(), _elements(move(elements), Base1::_lessThan), _previous(move(previous)), _next(move(next))
		{ }

		vector<Key> CollectKeys(vector<Key> previousNodesKeys = {}) const
		{
			auto ks = this->GetKeysFrom(_elements);
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
				throw out_of_range("Type not support GetRawPtr");
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
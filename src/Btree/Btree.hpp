#pragma once
/***********************************************************************************************************
   Btree class in Collections
***********************************************************************************************************/

#include <vector>
#include <functional>
#include <memory>
#include <utility>
#include <algorithm>
#include <array>
#include "Basic.hpp"
#include "Enumerator.hpp"
#include "NodeFactory.hpp"
#include "Exception.hpp"

namespace Collections
{
	using ::std::function;
	using ::std::array;
	using ::std::pair;
	using ::std::make_pair;
	using ::std::vector;
	using ::std::sort;
	using ::std::unique_ptr;
	using ::std::make_shared;
	using ::std::size_t;
	using ::std::move;
	using ::std::make_index_sequence;
	using ::std::index_sequence;

	template <auto Total, auto ItemCapacity>
	struct PerNodeCountGenerator
	{
		constexpr static int Current = Total == 0 ? 
			0 : (Total % ItemCapacity == 0 ? 
				ItemCapacity : ((Total % ((Total / ItemCapacity) + 1) == 0 ? 
					(Total / ((Total / ItemCapacity) + 1)) : ((Total / ((Total / ItemCapacity) + 1)) + 1))));
		using Next = PerNodeCountGenerator<Total - Current, ItemCapacity>;
	};

	template <auto ItemCapacity>
	struct PerNodeCountGenerator<0, ItemCapacity>
	{
		constexpr static int Current = 0;
	};

	template <auto Index, auto Total, auto ItemCapacity>
	struct PositionGetter
	{
		using Position = typename PositionGetter<Index - 1, Total, ItemCapacity>::Position::Next;
	};

	template <auto Total, auto ItemCapacity>
	struct PositionGetter<0, Total, ItemCapacity>
	{
		using Position = PerNodeCountGenerator<Total, ItemCapacity>;
	};

	template <auto Total, auto ItemCapacity, auto Index>
	constexpr auto GetItemsCount()
	{
		return PositionGetter<Index, Total, ItemCapacity>::Position::Current;
	}

	/*template <auto Total, auto ItemCapacity, size_t I, size_t... Is>
	constexpr auto GetPreItemsCountHelper()
	{
		if constexpr (sizeof...(Is) == 0) 
		{
			return GetItemsCount<Total, ItemCapacity, I>();
		}
		else
		{
			return GetItemsCount<Total, ItemCapacity, I>() + GetPreItemsCountHelper<Total, ItemCapacity, Is...>();
		}

	}*/

	template <auto Total, auto ItemCapacity, size_t... Is>
	constexpr auto GetPreItemsCountImp(index_sequence<Is...>)
	{
		return (0 + ... + GetItemsCount<Total, ItemCapacity, Is>());
		//return GetPreItemsCountHelper<Total, ItemCapacity, Is...>();
	}

	template <auto Total, auto ItemCapacity, auto Index>
	constexpr auto GetPreItemsCount()
	{
		return GetPreItemsCountImp<Total, ItemCapacity>(make_index_sequence<Index>());
	}

	template <auto Total, auto ItemCapacity>
	constexpr auto GetNodeCount()
	{
		return Total == 0 ?
			0 : (Total % ItemCapacity == 0 ? (Total / ItemCapacity) : (Total / ItemCapacity + 1));
	}

	/*template <bool Condition, typename A, typename B>
	struct CompileIf;

	template <typename A, typename B>
	struct CompileIf<true, A, B>
	{
		using Type = A;
	};

	template <typename A, typename B>
	struct CompileIf<true, A, B>
	{
		using Type = B;
	};*/
	
	template <order_int BtreeOrder, typename Key, typename Value>
	class Btree 
	{
	public:
		using _LessThan = LessThan<Key>;
	private:
		using Base   = NodeBase<Key, Value, BtreeOrder>;
		using NodeFactoryType = NodeFactory<Key, Value, BtreeOrder>;
		shared_ptr<_LessThan> _lessThanPtr;
		key_int              _keyCount{ 0 };
		unique_ptr<Base>     _root;

	public:
		Btree(_LessThan lessThan) : Btree(move(lessThan), array<pair<Key, Value>, 0>())
		{ }

		template <size_t NumOfEle>
		Btree(_LessThan lessThan, array<pair<Key, Value>, NumOfEle> keyValueArray)
			: _lessThanPtr(make_shared<_LessThan>(lessThan))
		{
			// 可以自己实现一个排序算法，这样找重复的容易些
			// 而且反正 pairArray 是在成功的情况下是要复制的，
			// 这个构造函数这也要复制，不如构造函数传引用，排序算法确定不重复的情况下，就直接复制到堆上
			// 可以确定好几个后一起构造
			sort(keyValueArray.begin(), keyValueArray.end(),
				 [&](const auto& p1, const auto& p2)
			{
				return lessThan(p1.first, p2.first);
			});

			if (Key* dupKeyPtr; DuplicateIn(keyValueArray, lessThan, dupKeyPtr))
			{
				throw DuplicateKeyException(move(*dupKeyPtr), "Duplicate key in constructor keyValueArray");
			}

			ConstructFromLeafToRoot(move(keyValueArray));
			_keyCount += NumOfEle;
		}

		// TODO Enumerator constructor
		// Btree(_LessThan lessThan, )

		Btree(Btree const& that)
			: _keyCount(that._keyCount), _root(that._root->Clone()), _lessThanPtr(that._lessThanPtr)
		{ }

		Btree(Btree&& that) noexcept
			: _keyCount(that._keyCount), _root(that._root.release()), _lessThanPtr(move(that._lessThanPtr))
		{
			that._keyCount = 0;
		}

		Btree& operator= (Btree const& that)
		{
			this->_root.reset(that._root->Clone());
			this->_keyCount = that._keyCount;
			this->_lessThanPtr = that._lessThanPtr;
		}

		Btree& operator= (Btree&& that) noexcept 
		{
			this->_root.reset(that._root.release());
			this->_keyCount = that._keyCount;
			that._keyCount = 0;
			this->_lessThanPtr = move(that._lessThanPtr);
		}

		bool ContainsKey(Key const& key) const
		{
			if (Empty()) { return false; }
			return _root->ContainsKey(key);
		}

		bool Empty() const
		{
			return _keyCount == 0;
		}

		key_int Count() const
		{
			return _keyCount;
		}

		vector<Key> Keys() const
		{
			return _root->Keys();
		}

#define EMPTY_CHECK if (Empty()) { throw KeyNotFoundException("The B+ tree is empty"); }
 		Value GetValue(Key const& key) const
		{
			EMPTY_CHECK;
			return _root->GetValue(key);
		}

		void ModifyValue(Key const& key, Value newValue)
		{
			EMPTY_CHECK;
			_root->ModifyValue(key, move(newValue));
		}

		void Remove(Key const& key)
		{
			EMPTY_CHECK;
			_root->Remove(key);
			--_keyCount;
		}
#undef EMPTY_CHECK

		void Add(pair<Key, Value> p)
		{
			_root->Add(move(p));
			++_keyCount;
		}

	private:		
		template <auto Total, auto Index, auto... Is>
		static void ForEachCons(function<void(int, int, int)> func)
		{
			func(Index, GetItemsCount<Total, BtreeOrder, Index>(), GetPreItemsCount<Total, BtreeOrder, Index>());
			if constexpr (sizeof...(Is) > 0)
			{
				ForEachCons<Total, Is...>(func);
			}
		}

		template <>
		static void ForEachCons<0, 0>(function<void(int, int, int)> func)
		{
			func(0, 0, 0);
		}

		template <typename T, auto Count, size_t... Is>
		static auto ConsNodeInArrayImp(array<T, Count> srcArray, shared_ptr<_LessThan> lessThan, index_sequence<Is...> is)
		{
			constexpr auto nodesCount = is.size() == 0 ? 1 : is.size();
			array<unique_ptr<Base>, nodesCount> consNodes;
			auto constor = [&srcArray, &consNodes, &lessThan](auto index, auto itemsCount, auto preItemsCount)
			{
				auto begin = srcArray.begin() + preItemsCount;
				auto end = begin + itemsCount;
				consNodes[index] = move(NodeFactoryType::MakeNode(CreateEnumerator(begin, end), lessThan));
			};
			if constexpr (is.size() == 0)
			{
				ForEachCons<0, 0>(constor);
			}
			else
			{
				ForEachCons<Count, Is...>(constor);
			}

			return consNodes;
		}

		template <typename T, auto Count>
		static auto ConsNodeInArray(array<T, Count> src, shared_ptr<_LessThan> lessThan)
		{
			return ConsNodeInArrayImp(move(src), move(lessThan), make_index_sequence<GetNodeCount<Count, BtreeOrder>()>());
		}

		template <bool FirstCall=true, typename T, size_t Count>
		void ConstructFromLeafToRoot(array<T, Count> ItemsToConsNode)
		{
			if constexpr (Count <= BtreeOrder)
			{
				_root = NodeFactoryType::MakeNode(CreateEnumerator(ItemsToConsNode.begin(), ItemsToConsNode.end()), _lessThanPtr);
				using ::std::placeholders::_1;
				using ::std::placeholders::_2;
				auto f1 = bind(&Btree::AddNodeCallback, this, _1, _2);
				auto f2 = bind(&Btree::DeleteNodeCallback, this, _1);
				_root->SetUpNodeCallback(f1, f2);
				return;
			}

			ConstructFromLeafToRoot<false>(move(ConsNodeInArray(move(ItemsToConsNode), _lessThanPtr)));
		}

		template <size_t Count>
		static bool DuplicateIn(array<pair<Key, Value>, Count>& sortedPairArray, 
								_LessThan const& lessThan, Key* & duplicateKey)
		{
			auto& array = sortedPairArray;
			if constexpr (Count > 1)
			{
				for (decltype(Count) i = 1; i < Count; ++i)
				{
					if (lessThan(array[i].first, array[i - 1].first) == lessThan(array[i - 1].first, array[i].first))
					{
						duplicateKey = &array[i].first;
						return true;
					}
				}
			}

			duplicateKey = nullptr;
			return false;
		}

		void AddNodeCallback(Base* srcNode, unique_ptr<Base> newNextNode)
		{
			array<unique_ptr<Base>, 2> nodes { move(_root), move(newNextNode) };
			_root = NodeFactoryType::MakeNode(CreateEnumerator(nodes), _lessThanPtr);
		}

		void DeleteNodeCallback(Base* node)
		{
			// TODO
		}
	};
}

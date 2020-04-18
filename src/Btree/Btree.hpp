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
#include "TreeInspector.hpp"
#include "../Basic/Exception.hpp"
#include "CollectionException.hpp"

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
	using ::std::make_unique;
	using ::std::size_t;
	using ::std::move;
	using ::std::make_index_sequence;
	using ::std::index_sequence;
	using ::Basic::KeyNotFoundException;
	using ::Basic::InvalidOperationException;
	using ::std::placeholders::_1;
	using ::std::placeholders::_2;

	template <auto Total, auto ItemCapacity>
	struct PerNodeCountGenerator
	{
		constexpr static int Current = 
								Total == 0 ? 
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

	template <auto Total, auto ItemCapacity, size_t... Is>
	constexpr auto GetPreItemsCountImp(index_sequence<Is...>)
	{
		return (0 + ... + GetItemsCount<Total, ItemCapacity, Is>());
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

	template <order_int BtreeOrder, typename Key, typename Value>
	class UniversalEnumerator;
	template <order_int BtreeOrder, typename Key, typename Value, template <typename> class Ptr>
	class Btree;
	template <typename T>
	struct DiskDataConverter;// 这个前置声明对吗，需要修饰命名空间吗
	template <order_int BtreeOrder, typename Key, typename Value, template <typename> class Ptr = unique_ptr>
	class Btree 
	{
	public:
		using _LessThan = LessThan<Key>;
	private:
		friend class UniversalEnumerator<BtreeOrder, Key, Value>;
		friend class DiskDataConverter<Btree>;// 这里不用 false 是因为这里是使用方，实现方需要根据不同情况写特化
		using Base   = NodeBase<Key, Value, BtreeOrder>;
		using NodeFactoryType = NodeFactory<Key, Value, BtreeOrder>;
		typename Base::UpNodeAddSubNodeCallback _addRootCallback = bind(&Btree::AddRootCallback, this, _1, _2);
		typename Base::UpNodeDeleteSubNodeCallback _deleteRootCallback = bind(&Btree::DeleteRootCallback, this, _1);
		typename Base::MinKeyChangeCallback _minKeyChangeCallback = bind(&Btree::RootMinKeyChangeCallback, this, _1, _2);
		typename Base::ShallowTreeCallback _shallowTreeCallback = bind(&Btree::ShallowRootCallback, this);
		shared_ptr<_LessThan> _lessThanPtr;
		key_int              _keyCount{ 0 };
		Ptr<Base>     _root;

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

		// TODO wait to test
		Btree(_LessThan lessThan, IEnumerator<pair<Key, Value>>& enumerator)
			: Btree(move(lessThan))
		{
			while (enumerator.MoveNext())
			{
				Add(enumerator.Current());
			}	
		}
		
		// TODO wait to test
		Btree(_LessThan lessThan, IEnumerator<pair<Key, Value>>&& enumerator)
			: Btree(move(lessThan))
		{ 
			while (enumerator.MoveNext())
			{
				Add(enumerator.Current());
			}
		}

		Btree(Btree const& that)
			: _keyCount(that._keyCount), _root(that._root->Clone()), _lessThanPtr(that._lessThanPtr)
		{
			this->SetRootCallbacks();
		}

		Btree(Btree&& that) noexcept
			: _keyCount(that._keyCount), _root(that._root.release()), _lessThanPtr(move(that._lessThanPtr))
		{
			this->SetRootCallbacks();
			that._keyCount = 0;
		}

		Btree& operator= (Btree const& that)
		{
			this->_root.reset(that._root->Clone());
			this->SetRootCallbacks();
			this->_keyCount = that._keyCount;
			this->_lessThanPtr = that._lessThanPtr;

			return *this;
		}

		Btree& operator= (Btree&& that) noexcept 
		{
			this->_root.reset(that._root.release());
			this->SetRootCallbacks();
			this->_keyCount = that._keyCount;
			that._keyCount = 0;
			this->_lessThanPtr = move(that._lessThanPtr);

			return *this;
		}

		void PrintTree()
		{
			// TODO this arg can be anything which like cout
			InspectNodeKeys(_root.get());
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
		Btree(Ptr<Base> root, key_int keyCount, shared_ptr<_LessThan> lessThan)
			: _root(move(root)), keyCount(keyCount), _lessThanPtr(lessThan)
		{ }

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
			array<Ptr<Base>, nodesCount> consNodes;
			auto constor = [&srcArray, &consNodes, &lessThan](auto index, auto itemsCount, auto preItemsCount)
			{
				auto begin = srcArray.begin() + preItemsCount;
				auto end = begin + itemsCount;
				consNodes[index] = NodeFactoryType::MakeNode(CreateEnumerator(begin, end), lessThan);
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
				this->SetRootCallbacks();
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

		void SetRootCallbacks()
		{
			_root->SetUpNodeCallback(&_addRootCallback, &_deleteRootCallback, &_minKeyChangeCallback);
			_root->SetShallowCallbackPointer(&_shallowTreeCallback);
		}

		// Below methods for root call
		void AddRootCallback(Base* srcNode, Ptr<Base> newNextNode)
		{
			// TODO Assert the srcNode == _root when debug
			_root->ResetShallowCallbackPointer();
			array<Ptr<Base>, 2> nodes { move(_root), move(newNextNode) }; // TODO have average?
			_root = NodeFactoryType::MakeNode(CreateEnumerator(nodes), _lessThanPtr);
			SetRootCallbacks();
		}

		void RootMinKeyChangeCallback(Key const&, Base*)
		{ }

		void DeleteRootCallback(Base* root)
		{
			if (root->Middle())
			{
				throw InvalidOperationException
					("MiddleNode root no need to call upper Delete node method, if called, means error");
			}
		}

		void ShallowRootCallback()
		{
			NodeFactoryType::TryShallow(_root, [this]()
			{
				this->SetRootCallbacks();
			});
		}
	};
}

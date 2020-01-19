#pragma once
#include <vector>
#include <functional>
#include <memory>
#include <utility>
#include <algorithm>
#include <array>
#include <exception>
#include "Basic.hpp"
#include "Enumerator.hpp"
#include "NodeFactory.hpp"
#include "Exception.hpp"
#include "SiblingFunc.hpp"

namespace Collections
{
	using ::std::function;
	using ::std::array;
	using ::std::pair;
	using ::std::make_pair;
	using ::std::vector;
	using ::std::sort;
	using ::std::exception;
	using ::std::runtime_error;
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
	private:
		using Base   = NodeBase  <Key, Value, BtreeOrder>;
		using Leaf   = LeafNode  <Key, Value, BtreeOrder>;
		using Middle = MiddleNode<Key, Value, BtreeOrder>;
		using NodeFactoryType = NodeFactory<Key, Value, BtreeOrder>;
		shared_ptr<typename Base::LessThan> _lessThanPtr;
		key_int              _keyNum{ 0 };
		unique_ptr<Base>     _root  { nullptr };

	public:
		using LessThan = typename Base::LessThan;

		template <size_t NumOfEle>
		Btree(LessThan lessThan, array<pair<Key, Value>, NumOfEle> keyValueArray)
			: _lessThanPtr(make_shared<LessThan>(lessThan))
		{
			if constexpr (NumOfEle == 0) { return; }

			// 可以自己实现一个排序算法，这样找重复的容易些
			// 而且反正 pairArray 是在成功的情况下是要复制的，
			// 这个构造函数这也要复制，不如构造函数传引用，排序算法确定不重复的情况下，就直接复制到堆上
			// 可以确定好几个后一起构造
			sort(keyValueArray.begin(), keyValueArray.end(),
				 [&](const auto& p1, const auto& p2)
			{
				return lessThan(p1.first, p2.first);
			});

			if (const Key *dupKeyPtr; DuplicateIn(keyValueArray, lessThan, dupKeyPtr))
			{
				throw DuplicateKeyException(*dupKeyPtr, "Duplicate key in constructor keyValueArray");
			}

			ConstructFromLeafToRoot(move(keyValueArray));
			_keyNum += NumOfEle;
		}

		// TODO Enumerator constructor
		// Btree(LessThan lessThan, )

		Btree(const Btree& that)
			: _keyNum(that._keyNum), _root(that._root->Clone())
		{ }

		Btree(Btree&& that) noexcept
			: _keyNum(that._keyNum), _root(that._root.release())
		{
			that._keyNum = 0;
		}

		Btree& operator=(Btree const & that)
		{
			this->_root.reset(that._root->Clone());
			this->_keyNum = that._keyNum;
			this->_lessThanPtr = that._lessThanPtr;
		}

		Btree& operator=(Btree&& that) noexcept 
		{
			this->_root.reset(that._root.release());
			this->_keyNum = that._keyNum;
			this->_lessThanPtr = that._lessThanPtr;
		}

		Value Search(Key const &key) const
		{
			if (Empty())
			{
				throw KeyNotFoundException("The tree is empty");
			}

			return _root->Search(key);
		}

		vector<Key> Explore() const 
		{
			vector<Key> keys;
			keys.reserve(_keyNum);
			TraverseLeaf([&keys](Leaf *l)
			{
				auto ks = l->AllKey();
				keys.insert(keys.end(), ks.begin(), ks.end());
			});

			return keys;
		}

		bool Have(const Key &key) const
		{
			if (!Empty())
			{
				return _root->Have(key);
			}

			return false;
		}

		bool Empty() const
		{
			return _keyNum == 0;
		}

		void Add(pair<Key, Value> p) 
		{
			if (Empty())
			{
				auto node = NodeFactoryType::MakeNode(&p, &p + 1, _lessThanPtr);
				_root.reset(node.release());
			}
			else
			{
				vector<Base *> passedNodeTrackStack;
				if (_root->Have(p.first, passedNodeTrackStack))
				{
					throw runtime_error("The key-value has already existed, can't be added.");
				}
				else
				{
					_root->Add(move(p), passedNodeTrackStack);
				}
			}

			++_keyNum;
		}

		// TODO tryAdd(pair<Key, Value>);
		void Modify(pair<Key, Value> pair)
		{
			if (!Empty())
			{
				_root->Modify(pair.first, move(pair.second));
			}
		}

		void Remove(const Key &key)
		{
			if (Empty())
			{
				return;
			}
			if (vector<Base*> passedNodeTrackStack;
				_root->Have(key, passedNodeTrackStack))
			{
				_root->Remove(key, passedNodeTrackStack);
				--_keyNum;
			}
		}

	private:
		void TraverseLeaf(function<void (Leaf *)> func) const
		{
			if (Empty())
			{
				return;
			}

			for (auto current = MinLeaf(); current != nullptr; current = current->nextLeaf())
			{
				func(current);
			}
		}

		Leaf* MinLeaf() const
		{
			return RecurSelectToGetLeaf(_root.get(), [](auto n)
			{
				return n->MinSon();
			});
		}
		
		template <auto Total, auto Num, auto... nums>
		static void ForEachCons(function<void(int, int, int)> func)
		{
			func(Num, GetItemsCount<Total, BtreeOrder, Num>(), GetPreItemsCount<Total, BtreeOrder, Num>());
			if constexpr (sizeof...(nums) > 0)
			{
				ForEachCons<Total, nums...>(func);
			}
		}

		template <typename T, auto Count, size_t... Is>
		static auto ConsNodeInArrayImp(array<T, Count> srcArray, shared_ptr<LessThan> lessThan, index_sequence<Is...> is)
		{
			array<pair<Key, unique_ptr<Base>>, is.size()> consNodes;
			ForEachCons<Count, Is...>([&srcArray, &consNodes, &lessThan](auto index, auto itemsCount, auto preItemsCount)
			{
				auto node = NodeFactoryType::MakeNode(CreateEnumerator(&srcArray[preItemsCount], &srcArray[preItemsCount + itemsCount]),
					lessThan);
				consNodes[index] = make_pair(move(node->MaxKey()), move(node));
			});

			return consNodes;
		}

		template <typename T, auto Count>
		static auto ConsNodeInArray(array<T, Count> src, shared_ptr<LessThan> lessThan)
		{
			return ConsNodeInArrayImp(move(src), move(lessThan), make_index_sequence<GetNodeCount<Count, BtreeOrder>()>());
		}

		template <bool FirstCall=true, typename T, size_t Count>
		void ConstructFromLeafToRoot(array<T, Count> ItemsToConsNode)
		{
			if constexpr (Count <= BtreeOrder)
			{
				_root = NodeFactoryType::MakeNode(ItemsToConsNode.begin(), ItemsToConsNode.end(), _lessThanPtr);
				return;
			}

			auto newNodes = ConsNodeInArray(move(ItemsToConsNode), _lessThanPtr);
			// TODO wait to set next leaf of leaf
			ConstructFromLeafToRoot<false>(move(newNodes));
		}

		template <size_t NumOfEle>
		static bool DuplicateIn(array<pair<Key, Value>, NumOfEle> const &sortedPairArray, LessThan const&
			lessThan, Key const *&duplicateKey)
		{
			auto &array = sortedPairArray;

			for (decltype(NumOfEle) i = 1; i < NumOfEle; ++i) 
			{
				if (lessThan(array[i].first, array[i - 1].first) == lessThan(array[i - 1].first, array[i].first))
				{
					duplicateKey = &array[i].first;
					return true;
				}
			}

			duplicateKey = nullptr;
			return false;
		}

		static Leaf* RecurSelectToGetLeaf(Base *node, function<Base* (Middle*)> choose)
		{
			while (node->Middle())
			{
				node = choose(static_cast<Middle *>(node));
			}

			return static_cast<Leaf *>(node);
		}
	};
}

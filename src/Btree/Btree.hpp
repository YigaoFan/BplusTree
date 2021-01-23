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
#include "../Basic/TypeTrait.hpp"
#include "Enumerator.hpp"
#include "Generator.hpp"
#include "Cloner.hpp"
#include "NodeFactory.hpp"
#include "TreeInspector.hpp"
#include "../Basic/Exception.hpp"
#include "CollectionException.hpp"
#include "../FuncLib/Persistence/FriendFuncLibDeclare.hpp"
#include "../FuncLib/Persistence/PtrSetter.hpp"
#include "../FuncLib/Store/TakeWithFile.hpp"

namespace Collections
{
	using Basic::FuncTraits;
	using Basic::InvalidOperationException;
	using Basic::KeyNotFoundException;
	using FuncLib::Store::TakeWithFile;
	using ::std::adjacent_find;
	using ::std::array;
	using ::std::function;
	using ::std::index_sequence;
	using ::std::make_index_sequence;
	using ::std::make_pair;
	using ::std::make_shared;
	using ::std::make_unique;
	using ::std::move;
	using ::std::pair;
	using ::std::remove_cvref_t;
	using ::std::size_t;
	using ::std::sort;
	using ::std::unique_ptr;
	using ::std::vector;
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

	// 这三个类型参数的顺序要不要调整下啊
	template <order_int BtreeOrder, typename Key, typename Value, StorePlace Place = StorePlace::Memory>
	class Btree : public TakeWithFile<IsDisk<Place> ? Switch::Enable : Switch::Disable>
	{
	public:
		using _LessThan = LessThan<Key>;
	private:
		// 待验证：因为它是某个模板的特化，所以模板需要前置声明，特化不需要
		template <typename... Ts>
		using Ptr = typename TypeConfig::template Ptr<Place>::template Type<Ts...>;
		friend struct FuncLib::Persistence::ByteConverter<Btree, false>; // Btree here is undefined or incomplete type
		friend struct FuncLib::Persistence::TypeConverter<Btree>;
		friend struct FuncLib::Persistence::TypeConverter<Btree<BtreeOrder, Key, Value, StorePlace::Memory>>;
		using Base = TakeWithFile<IsDisk<Place> ? Switch::Enable : Switch::Disable>;
		using Node = NodeBase<Key, Value, BtreeOrder, Place>;
		using StoredKey = typename Node::StoredKey;
		using StoredValue = typename Node::StoredValue;
		using NodeFactoryType = NodeFactory<Key, Value, BtreeOrder, Place>;
		typename Node::UpNodeAddSubNodeCallback const _addRootCallback = bind(&Btree::AddRootCallback, this, _1, _2);
		typename Node::UpNodeDeleteSubNodeCallback const _deleteRootCallback = bind(&Btree::DeleteRootCallback, this, _1);
		typename Node::MinKeyChangeCallback const _minKeyChangeCallback = bind(&Btree::RootMinKeyChangeCallback, this, _1, _2);
		typename Node::ShallowTreeCallback const _shallowTreeCallback = bind(&Btree::ShallowRootCallback, this);
		key_int              _keyCount{ 0 };
		Ptr<Node>            _root;

	public:
		Btree() : Btree(array<pair<StoredKey, StoredValue>, 0>())
		{ }

		/// 只有这一种构造才能保证，Node 被构造出来携带正确的 DiskPos
		Btree(File* file)
			: Base(file),
			  _root(ConstructRoot(array<pair<StoredKey, StoredValue>, 0>())),
			  _keyCount(0)
		{
			static_assert(Place == StorePlace::Disk, "Only Btree on disk can call this method");
		}

		template <size_t NumOfEle>
		Btree(array<pair<StoredKey, StoredValue>, NumOfEle> keyValueArray)
			: _root(ConstructRoot(move(keyValueArray))),
			  _keyCount(NumOfEle)
		{
			this->SetRootCallbacks();
		}

		// TODO wait to test
		Btree(IEnumerator<pair<StoredKey, StoredValue>> auto enumerator) : Btree()
		{
			while (enumerator.MoveNext())
			{
				Add(enumerator.Current());
			}	
		}

		Btree(Btree const& that)
			: _keyCount(that._keyCount), _root(Clone(that._root.get()))
		{
			this->SetRootCallbacks();
		}

		Btree(Btree&& that) noexcept
			: Base(move(that)), _keyCount(that._keyCount), _root(move(that._root))
		{
			this->SetRootCallbacks();
			that._keyCount = 0;
		}

		Btree& operator= (Btree const& that)
		{
			this->_root.reset(Clone(that._root.get()));// TODO reset is unique_ptr
			this->SetRootCallbacks();
			this->_keyCount = that._keyCount;

			return *this;
		}

		Btree& operator= (Btree&& that) noexcept 
		{
			this->_root.reset(that._root.release());
			this->SetRootCallbacks();
			this->_keyCount = that._keyCount;
			that._keyCount = 0;

			return *this;
		}

		void CheckTree()
		{
			InspectNodeKeys<Key, Value, BtreeOrder, Place>(_root.get());
		}

#define ARG_TYPE_IN_NODE(METHOD, IDX) typename FuncTraits<typename GetMemberFuncType<decltype(&Node::METHOD)>::Result>::template Arg<IDX>::Type
		bool ContainsKey(ARG_TYPE_IN_NODE(ContainsKey, 0) key) const
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
			return _root->LetMinLeafCollectKeys();
		}

#define EMPTY_CHECK if (Empty()) { throw KeyNotFoundException("The B+ tree is empty"); }
		Value GetValue(ARG_TYPE_IN_NODE(GetValue, 0) key) const
		{
			EMPTY_CHECK;
			return _root->GetValue(key);
		}

		void ModifyValue(ARG_TYPE_IN_NODE(ModifyValue, 0) key, ARG_TYPE_IN_NODE(ModifyValue, 1) newValue)
		{
			EMPTY_CHECK;
			_root->ModifyValue(key, move(newValue));
		}

		void ModifyKey(ARG_TYPE_IN_NODE(ModifyValue, 0) oldOey, StoredKey newKey)
		{
			EMPTY_CHECK;
			auto v = move(_root->GetValue(oldOey));
			_root->Remove(oldOey);
			--_keyCount;

			_root->Add({ move(newKey), move(v) });
			++_keyCount;
		}

		void Remove(ARG_TYPE_IN_NODE(Remove, 0) key)
		{
			EMPTY_CHECK;
			_root->Remove(key);
			--_keyCount;
		}
#undef EMPTY_CHECK

		void Add(ARG_TYPE_IN_NODE(Add, 0) p)
		{
			_root->Add(move(p));
			++_keyCount;
		}
#undef ARG_TYPE_IN_NODE

		RecursiveGenerator<pair<StoredKey, StoredValue>*> GetStoredPairEnumerator()
		{
			return _root->GetStoredPairEnumerator();
		}

	private:
		Btree(key_int keyCount, Ptr<Node> root, File* file) : Base(file), _root(move(root)), _keyCount(keyCount)
		{
			static_assert(Place == StorePlace::Disk, "Only Btree on disk can call this method");
			this->SetRootCallbacks();
		}

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
		auto ConsNodeInArrayImp(array<T, Count> srcArray, index_sequence<Is...> is)
		{
			constexpr auto nodesCount = is.size() == 0 ? 1 : is.size();
			array<Ptr<Node>, nodesCount> consNodes;
			auto constor = [&srcArray, &consNodes, this](auto index, auto itemsCount, auto preItemsCount)
			{
				auto begin = srcArray.begin() + preItemsCount;
				auto end = begin + itemsCount;
				consNodes[index] = this->MakeNewNode(CreateMoveEnumerator(begin, end));
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
		auto ConsNodeInArray(array<T, Count> src)
		{
			return ConsNodeInArrayImp(move(src), make_index_sequence<GetNodeCount<Count, BtreeOrder>()>());
		}

		template <size_t NumOfEle>
		decltype(_root) ConstructRoot(array<pair<StoredKey, StoredValue>, NumOfEle> keyValueArray)
		{
			// Check duplicate
			auto less = [&](auto const& p1, auto const& p2)
			{
				return static_cast<Key const&>(p1.first) < static_cast<Key const&>(p2.first);
			};

			sort(keyValueArray.begin(), keyValueArray.end(), less);

			auto equal = [&](auto const &p1, auto const &p2)
			{
				return less(p1, p2) == less(p2, p1);
			};

			auto last = adjacent_find(keyValueArray.begin(), keyValueArray.end(), equal);
			if (last != keyValueArray.end())
			{
				throw DuplicateKeyException(move(last->first), "Duplicate key in constructor keyValueArray");
			}

			return ConstructFromLeafToRoot(move(keyValueArray));
		}

		template <typename T, size_t Count>
		decltype(_root) ConstructFromLeafToRoot(array<T, Count> ItemsToConsNode)
		{
			if constexpr (Count <= BtreeOrder)
			{
				auto root = MakeNewNode(CreateMoveEnumerator(ItemsToConsNode.begin(), ItemsToConsNode.end()));
				return root;
			}

			return ConstructFromLeafToRoot(move(ConsNodeInArray(move(ItemsToConsNode))));
		}

		void SetRootCallbacks()
		{
			SET_PROPERTY(_root, &, ->SetUpNodeCallback(&_addRootCallback, &_deleteRootCallback, &_minKeyChangeCallback));
			SET_PROPERTY(_root, &, ->SetShallowCallbackPointer(&_shallowTreeCallback));
		}

		// Below methods for root callback
		void AddRootCallback(Node* srcNode, Ptr<Node> newNextNode)
		{
			// TODO Assert the srcNode == _root when debug
			_root->ResetShallowCallbackPointer();
			array<Ptr<Node>, 2> nodes { move(_root), move(newNextNode) }; // TODO have average node count between nodes?
			_root = MakeNewNode(CreateMoveEnumerator(nodes));
			SetRootCallbacks();
		}

		void RootMinKeyChangeCallback(result_of_t<decltype(&Node::MinKey)(Node)>, Node*)
		{ }

		void DeleteRootCallback(Node* root)
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

		template <typename... Args>
		Ptr<Node> MakeNewNode(Args&&... args)
		{
			if constexpr (Place == StorePlace::Disk)
			{
				auto f = this->GetLessOwnershipFile();
				return NodeFactoryType::MakeNodeOnDisk(f, forward<Args>(args)...);
			}
			else
			{
				return NodeFactoryType::MakeNodeOnMemory(forward<Args>(args)...);
			}
		}
	};
}

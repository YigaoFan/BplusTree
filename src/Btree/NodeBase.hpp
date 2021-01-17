#pragma once
/***********************************************************************************************************
 Abstract class NodeBase in Collections
***********************************************************************************************************/

#include <vector>
#include <memory>
#include <functional>
#include <type_traits>
#include "Basic.hpp"
#include "Generator.hpp"
#include "../FuncLib/Persistence/FriendFuncLibDeclare.hpp"
#include "../Basic/TypeTrait.hpp"
#include "TypeConfig.hpp"

namespace Collections
{
	using ::Basic::IsSpecialization;
	using ::FuncLib::Persistence::UniqueDiskPtr;
	using ::FuncLib::Persistence::MakeUnique;
	using ::std::function;
	using ::std::move;
	using ::std::pair;
	using ::std::remove_const_t;
	using ::std::remove_pointer_t;
	using ::std::result_of_t;
	using ::std::shared_ptr;
	using ::std::unique_ptr;
	using ::std::vector;

	enum Position
	{
		Previous,
		Next,
	};

	template <StorePlace Place>
	constexpr bool IsDisk = Place == StorePlace::Disk;

	template <typename Key, typename Value, order_int BtreeOrder, StorePlace Place = StorePlace::Memory>
	class NodeBase
	{
	private:
		template <typename... Ts>
		using Ptr = typename TypeConfig::template Ptr<Place>::template Type<Ts...>;
	public:
		virtual typename TypeSelector<Place, Refable::Yes, Key>::Result MinKey() const = 0;

	public:
		using UpNodeAddSubNodeCallback = function<void(NodeBase*, Ptr<NodeBase>)> const;
		using UpNodeDeleteSubNodeCallback = function<void(NodeBase*)> const;
		using MinKeyChangeCallback = function<void(result_of_t<decltype(&NodeBase::MinKey)(NodeBase)>, NodeBase*)> const;
		using ShallowTreeCallback = function<void()> const;
		template <typename T>
		using OwnerLessPtr = typename TypeSelector<Place, Refable::No, T*>::Result;

	protected:
		UpNodeAddSubNodeCallback* _upNodeAddSubNodeCallbackPtr = nullptr;
		UpNodeDeleteSubNodeCallback* _upNodeDeleteSubNodeCallbackPtr = nullptr;
		MinKeyChangeCallback* _minKeyChangeCallbackPtr = nullptr;

	public:
		static constexpr order_int LowBound = 1 + ((BtreeOrder - 1) / 2);
		void SetUpNodeCallback(UpNodeAddSubNodeCallback* addSubNodeCallbackPtr,
			UpNodeDeleteSubNodeCallback* deleteSubNodeCallbackPtr,
			MinKeyChangeCallback* minKeyChangeCallbackPtr)
		{
			_upNodeAddSubNodeCallbackPtr = addSubNodeCallbackPtr;
			_upNodeDeleteSubNodeCallbackPtr = deleteSubNodeCallbackPtr;
			_minKeyChangeCallbackPtr = minKeyChangeCallbackPtr;
		}

		virtual void SetShallowCallbackPointer(ShallowTreeCallback*)
		{ }
		virtual void ResetShallowCallbackPointer()
		{ }
		virtual void LessThanPredicate(shared_ptr<LessThan<Key>>) = 0;
		virtual ~NodeBase() = default;
		virtual bool Middle() const = 0;
		virtual vector<Key> Keys() const = 0;
		/// same as in LeafNode
		using StoredKey = typename TypeSelector<Place, Refable::No, Key>::Result;
		using StoredValue = typename TypeSelector<Place, Refable::No, Value>::Result;

#define KEY_T StoredKey
#define VALUE_T StoredValue
		virtual bool ContainsKey(Key const& key) const = 0;
		virtual StoredValue& GetValue(Key const& key) = 0;
		virtual void ModifyValue(Key const& key, VALUE_T) = 0;
		virtual void Add(pair<KEY_T, VALUE_T>) = 0;
		virtual void Remove(Key const& key) = 0;
#undef VALUE_T
#undef KEY_T
		virtual vector<Key> KeysInThisNode() const = 0;
		virtual vector<OwnerLessPtr<NodeBase>> SubNodes() const = 0;
		virtual RecursiveGenerator<pair<StoredKey, StoredValue>*> GetStoredPairEnumerator() = 0;

	protected:
		static Position ChooseAddPosition(order_int preCount, order_int thisCount, order_int nxtCount)
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

		static Position ChooseRemovePosition(order_int preCount, order_int thisCount, order_int nxtCount)
		{
			return ChooseAddPosition(preCount, thisCount, nxtCount) == Position::Previous ? Position::Next : Position::Previous;
		}

		static auto CopyNode(auto thisPtr)
		{
			using NodeType = remove_const_t<remove_pointer_t<decltype(thisPtr)>>;

			if constexpr (IsSpecialization<Ptr<int>, UniqueDiskPtr>::value)
			{
				auto f = thisPtr->GetLessOwnershipFile();
				return MakeUnique(*thisPtr, f);
			}
			else
			{
				return make_unique<NodeType>(*thisPtr);
			}
		}

		static auto NewEmptyNode(auto thisPtr, auto lessThanPred)
		{
			using NodeType = remove_const_t<remove_pointer_t<decltype(thisPtr)>>;

			if constexpr (IsSpecialization<Ptr<int>, UniqueDiskPtr>::value)
			{
				auto f = thisPtr->GetLessOwnershipFile();
				auto n = NodeType(lessThanPred);
				// 或许 MakeUnique 可以实现成原位构造那样？ TODO
				return MakeUnique(move(n), f);
			}
			else
			{
				return make_unique<NodeType>(lessThanPred);
			}
		}

		/// Will convert StoredKey to Key
		template <typename Elements>
		static vector<Key> GetKeysFrom(Elements const& elements)
		{
			vector<Key> ks;
			ks.reserve(elements.Count());

			for (auto& x : elements)
			{
				ks.push_back(x.first);
			}

			return ks;
		}
	};
}
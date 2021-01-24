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
#include "Elements.hpp"

namespace Collections
{
	using ::Basic::IsSpecialization;
	using ::FuncLib::Persistence::MakeUnique;
	using ::FuncLib::Persistence::UniqueDiskPtr;
	using ::std::function;
	using ::std::make_unique;
	using ::std::move;
	using ::std::pair;
	using ::std::remove_const_t;
	using ::std::remove_pointer_t;
	using ::std::result_of_t;
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
		inline static LessThan<Key>* _lessThan = +[](Key const &k1, Key const &k2) { return k1 < k2; };

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
		virtual ~NodeBase() = default;
		virtual bool Middle() const = 0;
		virtual vector<Key> LetMinLeafCollectKeys() const = 0;
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

		template <typename Node>
		static auto NewEmptyNode(Node* thisPtr)
		{
			if constexpr (IsSpecialization<Ptr<int>, UniqueDiskPtr>::value)
			{
				auto f = thisPtr->GetLessOwnershipFile();
				auto n = Node();
				// 或许 MakeUnique 可以实现成原位构造那样？ TODO
				return MakeUnique(move(n), f);
			}
			else
			{
				return make_unique<Node>();
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

		template <bool IsLeaf, typename Node>
		static void AddWith(OwnerLessPtr<Node> previous, OwnerLessPtr<Node> next, Node *self, auto p)
		{
			unsigned char state = 0;
			constexpr unsigned char previousValidFlag = 0b0000'0001;
			constexpr unsigned char nextValidFlag = 0b0000'0010;

			auto setNextFlag = [&state](bool valid)
			{
				if (valid)
				{
					state |= nextValidFlag;
				}
				else
				{
					state &= ~nextValidFlag;
				}
			};
			auto setPreviousFlag = [&state](bool valid)
			{
				if (valid)
				{
					state |= previousValidFlag;
				}
				else
				{
					state &= ~previousValidFlag;
				}
			};

			setPreviousFlag(previous != nullptr and not previous->_elements.Full());
			setNextFlag(next != nullptr and not next->_elements.Full());

			switch (state)
			{
			case 0: goto ConsNewNode;
			case 1: goto AddToPre;
			case 2: goto AddToNext;
			case 3:
			{
				switch (ChooseAddPosition(previous->_elements.Count(), self->_elements.Count(),
										  next->_elements.Count()))
				{
				case Position::Previous: goto AddToPre;
				case Position::Next:     goto AddToNext;
				}
			}

			default:
				throw std::out_of_range("add state out of range");
			}

		AddToPre:	
			previous->Append(self->ExchangeMin(move(p)));
			return;

		AddToNext:
			next->EmplaceHead(self->ExchangeMax(move(p)));
			return;

		ConsNewNode:
			auto newNxtNode = self->NewEmptyNode(self);
			if constexpr (IsLeaf)
			{
				self->SetRelationWhileSplitNewNext(newNxtNode.get());
			}

			auto i = self->_elements.SelectBranch(p.first);
			constexpr auto middle = BtreeOrder / 2;
			if (i <= (middle - 1))
			{
				auto items = self->_elements.PopOutItems(BtreeOrder - middle);
				self->ProcessedAdd(move(p)); /* Add (Does it duplicate to SelectBranch before)*/
				newNxtNode->AppendItems(move(items));
			}
			else
			{
				auto items = self->_elements.PopOutItems(middle);
				newNxtNode->AppendItems(move(items));
				newNxtNode->ProcessedAdd(move(p));
			}

			(*self->_upNodeAddSubNodeCallbackPtr)(self, move(newNxtNode));
		}

		// 给 order 加个限制 static_assert(order > ...)
		template <bool IsLeaf, typename Node>
		void AdjustAfterRemove(OwnerLessPtr<Node> previous, OwnerLessPtr<Node> next, Node* self, auto noWhereToProcessCallback)
		{
			if (self->_elements.Empty())
			{
				// 这里会不会是有问题的，一个节点除了在 Order 为 2 的情况下，会出现 1 外，什么情况下会只有一个节点
				printf("delete self");
				(*self->_upNodeDeleteSubNodeCallbackPtr)(self);
			}

			unsigned char state = 0;
			constexpr unsigned char previousValidFlag = 0b0000'0001;
			constexpr unsigned char nextValidFlag = 0b0001'0000;

			auto addNextState = [&state](bool valid)
			{
				if (valid)
				{
					state += nextValidFlag;
				}
			};
			auto addPreviousState = [&state](bool valid)
			{
				if (valid)
				{
					state += previousValidFlag;
				}
			};

			if (previous != nullptr)
			{
				addPreviousState(true);
				addPreviousState(previous->_elements.Count() > LowBound);
			}
			if (next != nullptr)
			{
				addNextState(true);
				addNextState(next->_elements.Count() > LowBound);
			}

			// steal first? 优先 2
			switch (state)
			{
			// 这里有两种选择，combine 和 steal
			case 0x00: goto NoWhereToProcess;
			case 0x01: goto CombineWithPrevious;
			case 0x02: goto StealPrevious;
			case 0x10: goto CombineWithNext;
			case 0x11: goto CombineWithPrevious; // combine with preivous or next, 选择回调在自身结点上调用，因为考虑硬盘 B+ 树， node 自身已读
			case 0x12: goto StealPrevious;

			case 0x20:
			case 0x21: goto StealNext;
			case 0x22:
			{
				switch (ChooseRemovePosition(previous->_elements.Count(), self->_elements.Count(),
											 next->_elements.Count()))
				{
				case Position::Next:     goto StealNext;
				case Position::Previous: goto StealPrevious;
				}
			}

			default:
				throw std::out_of_range("after remove state out of range");
			}

		

		CombineWithNext:
		{
			if constexpr (IsLeaf)
			{
				self->SetRelationWhileCombineNext(next);
			}
			auto items = next->_elements.PopOutAll();
			(*next->_upNodeDeleteSubNodeCallbackPtr)(next); /*TODO 这里的参数有时是指针有时是硬存指针*/
			self->AppendItems(move(items));
			return;
		}

		CombineWithPrevious:
		{
			if constexpr (IsLeaf)
			{
				self->SetRelationWhileCombineToPrevious(previous);
			}
			auto items = self->_elements.PopOutAll();
			(*self->_upNodeDeleteSubNodeCallbackPtr)(self);
			previous->AppendItems(move(items));
			return;
		}
		StealNext:
		{
			auto item = next->_elements.FrontPopOut();
			(*next->_minKeyChangeCallbackPtr)(next->MinKey(), next);
			self->Append(move(item));
			return;
		}
		StealPrevious:
		{
			auto item = previous->_elements.PopOut();
			self->EmplaceHead(move(item));
			return;
		}
		NoWhereToProcess:
			noWhereToProcessCallback();
		}
	};
}
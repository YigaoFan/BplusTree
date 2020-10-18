#pragma once
/***********************************************************************************************************
 Abstract class NodeBase in Collections
***********************************************************************************************************/

#include <vector>
#include <memory>
#include <functional>
#include <type_traits>
#include "Basic.hpp"
#include "../FuncLib/FriendFuncLibDeclare.hpp"
#include "../FuncLib/Store/FileResource.hpp"
#include "../Basic/TypeTrait.hpp"
#include "TypeConfig.hpp"

namespace Collections
{
	using ::Basic::IsSpecialization;
	using ::FuncLib::UniqueDiskPtr;
	using ::std::function;
	using ::std::move;
	using ::std::remove_const_t;
	using ::std::remove_pointer_t;
	using ::std::result_of_t;
	using ::std::unique_ptr;
	using ::std::vector;

	enum Position
	{
		Previous,
		Next,
	};

	template <template <typename...> class Ptr>
	constexpr bool IsDisk = IsSpecialization<Ptr<int>, UniqueDiskPtr>::value;

	template <template <typename...> class Ptr>
	constexpr StorePlace GetStorePlace = IsDisk<Ptr> ? StorePlace::Disk : StorePlace::Memory;

	template <typename Key, typename Value, order_int BtreeOrder, template <typename...> class Ptr = unique_ptr>
	class NodeBase
	{
	public:
		virtual typename TypeSelector<GetStorePlace<Ptr>, Refable::Yes, Key>::Result MinKey() const = 0;
	public:
		// 下面这些 NodeBase 指针应该不需要成为特殊的硬盘指针 TODO
		using UpNodeAddSubNodeCallback = function<void(NodeBase*, Ptr<NodeBase>)>;
		using UpNodeDeleteSubNodeCallback = function<void(NodeBase*)>;
		using MinKeyChangeCallback = function<void(result_of_t<decltype(&NodeBase::MinKey)(NodeBase)>, NodeBase*)>;
		using ShallowTreeCallback = function<void()>;
		template <typename T>
		using OwnerLessPtr = typename TypeSelector<GetStorePlace<Ptr>, Refable::No, T*>::Result;

	protected:
		static constexpr order_int LowBound = 1 + ((BtreeOrder - 1) / 2);
		UpNodeAddSubNodeCallback* _upNodeAddSubNodeCallbackPtr = nullptr;
		UpNodeDeleteSubNodeCallback* _upNodeDeleteSubNodeCallbackPtr = nullptr;
		MinKeyChangeCallback* _minKeyChangeCallbackPtr = nullptr;

	public:
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
		virtual Ptr<NodeBase> Clone() const = 0;
		virtual ~NodeBase() = default;
		virtual bool Middle() const = 0;
		virtual vector<Key> Keys() const = 0;
		/// same as in LeafNode
		using StoredKey = typename TypeSelector<GetStorePlace<Ptr>, Refable::No, Key>::Result;

#define CONST_KEY_REF_T StoredKey const&
#define KEY_T StoredKey
		virtual bool ContainsKey(CONST_KEY_REF_T) const = 0;
		virtual Value GetValue(CONST_KEY_REF_T) const = 0;
		virtual void ModifyValue(CONST_KEY_REF_T, Value) = 0;
		virtual void Add(pair<KEY_T, Value>) = 0;
		virtual void Remove(CONST_KEY_REF_T) = 0;
#undef KEY_T
#undef CONST_KEY_REF_T
		virtual vector<Key> SubNodeMinKeys() const = 0;
		virtual vector<OwnerLessPtr<NodeBase>> SubNodes() const = 0;

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
	};
}

#define DEF_LESS_THAN_SETTER \
void LessThanPredicate(shared_ptr<LessThan<Key>> lessThan) override\
{\
	_elements.LessThanPtr = lessThan;\
}

#define DEF_COPY_NODE                                                         \
	static auto CopyNode(auto thisPtr)                                        \
	{                                                                         \
		using NodeType = remove_const_t<remove_pointer_t<decltype(thisPtr)>>; \
                                                                              \
		if constexpr (IsSpecialization<Ptr<int>, UniqueDiskPtr>::value)       \
		{                                                                     \
			using FuncLib::Store::FileResource;                               \
			auto f = FileResource::GetCurrentThreadFile().get();              \
			return UniqueDiskPtr<NodeType>::MakeUnique(*thisPtr, f);          \
		}                                                                     \
		else                                                                  \
		{                                                                     \
			return make_unique<NodeType>(*thisPtr);                           \
		}                                                                     \
	}

#define DEF_NEW_EMPTY_NODE                                                    \
	static auto NewEmptyNode(auto thisPtr)                                    \
	{                                                                         \
		using NodeType = remove_const_t<remove_pointer_t<decltype(thisPtr)>>; \
                                                                              \
		if constexpr (IsSpecialization<Ptr<int>, UniqueDiskPtr>::value)       \
		{                                                                     \
			using FuncLib::Store::FileResource;                               \
			auto f = FileResource::GetCurrentThreadFile().get();              \
			auto n = NodeType(thisPtr->_elements.LessThanPtr);                \
			return UniqueDiskPtr<NodeType>::MakeUnique(move(n), f);           \
		}                                                                     \
		else                                                                  \
		{                                                                     \
			return make_unique<NodeType>(thisPtr->_elements.LessThanPtr);     \
		}                                                                     \
	}

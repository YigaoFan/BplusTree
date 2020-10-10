#pragma once
/***********************************************************************************************************
 Abstract class NodeBase in Collections
***********************************************************************************************************/

#include <vector>
#include <memory>
#include <functional>
#include <tuple>
#include <type_traits>
#include "Basic.hpp"
#include "../FuncLib/FriendFuncLibDeclare.hpp"
#include "../FuncLib/Store/FileResource.hpp"
#include "../Basic/TypeTrait.hpp"

namespace Collections
{
	using ::Basic::CompileIf;
	using ::Basic::IsSpecialization;
	using ::FuncLib::UniqueDiskPtr;
	using ::FuncLib::TypeConverter;
	using ::std::function;
	using ::std::is_fundamental_v;
	using ::std::make_tuple;
	using ::std::move;
	using ::std::reference_wrapper;
	using ::std::unique_ptr;
	using ::std::vector;

	enum Position
	{
		Previous,
		Next,
	};

	template <template <typename...> class Ptr>
	constexpr bool IsDisk = IsSpecialization<Ptr<int>, UniqueDiskPtr>::value;

	template <bool IsDisk, bool RefUsable, typename T>
	struct DataType;

	template <typename Ty, bool RefUsable>
	struct DataType<true, RefUsable, Ty>
	{
		using T = typename TypeConverter<Ty>::To;
	};

	template <typename Ty>
	struct DataType<false, false, Ty>
	{
		using T = Ty;
	};

	template <typename Ty>
	struct DataType<false, true, Ty>
	{
		using T = typename CompileIf<is_fundamental_v<Ty>, Ty, reference_wrapper<Ty const>>::Type;
	};

	template <typename Key, typename Value, order_int BtreeOrder, template <typename...> class Ptr = unique_ptr>
	class NodeBase
	{
	public:
		using UpNodeAddSubNodeCallback = function<void(NodeBase*, Ptr<NodeBase>)>;
		using UpNodeDeleteSubNodeCallback = function<void(NodeBase*)>;
		using MinKeyChangeCallback = function<void(Key const&, NodeBase*)>;
		using ShallowTreeCallback = function<void()>;

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
		// MiddleNode elements key type is same as below method return type
		virtual typename DataType<IsDisk<Ptr>, true, Key>::T const MinKey() const = 0;
		virtual bool ContainsKey(Key const&) const = 0;
		virtual Value GetValue(Key const&) const = 0;
		virtual void ModifyValue(Key const&, Value) = 0;
		virtual void Add(pair<Key, Value>) = 0;
		virtual void Remove(Key const&) = 0;
		virtual vector<Key> SubNodeMinKeys() const = 0;
		virtual vector<NodeBase*> SubNodes() const = 0;
		
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
			auto f = FileResource::GetCurrentThreadFile();                    \
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
			auto f = FileResource::GetCurrentThreadFile();                    \
			auto n = NodeType(thisPtr->_elements.LessThanPtr);                \
			return UniqueDiskPtr<NodeType>::MakeUnique(move(n), f);           \
		}                                                                     \
		else                                                                  \
		{                                                                     \
			return make_unique<NodeType>(thisPtr->_elements.LessThanPtr);     \
		}                                                                     \
	}

#pragma once
/***********************************************************************************************************
 Abstract class NodeBase in Collections
***********************************************************************************************************/

#include <vector>
#include <memory>
#include <functional>
#include <tuple>
#include "Basic.hpp"

namespace Collections
{
	using ::std::move;
	using ::std::unique_ptr;
	using ::std::vector;
	using ::std::function;
	using ::std::make_tuple;

	enum Position
	{
		Previous,
		Next,
	};
	enum Operation
	{
		Add,
		Remove,
	};

	template <typename Key, typename Value, order_int BtreeOrder>
	class NodeBase
	{
	public:
		using UpNodeAddSubNodeCallback = function<void(NodeBase*, unique_ptr<NodeBase>)>;
		using UpNodeDeleteSubNodeCallback = function<void(NodeBase*)>;
		using MinKeyChangeCallback = function<void(Key const&, NodeBase*)>;
		using ShallowTreeCallback = function<void()>;

	protected:
		static const order_int LowBound = 1 + ((BtreeOrder - 1) / 2);
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
		virtual unique_ptr<NodeBase> Clone() const = 0;
		virtual ~NodeBase() = default;
		virtual bool Middle() const = 0;
		virtual vector<Key> Keys() const = 0;
		virtual Key const& MinKey() const = 0;
		virtual bool ContainsKey(Key const&) const = 0;
		virtual Value GetValue(Key const&) const = 0;
		virtual void ModifyValue(Key const&, Value) = 0;
		virtual void Add(pair<Key, Value>) = 0;
		virtual void Remove(Key const&) = 0;
		// TODO Could use below method implement Enumerator: use LeafNode relation save state
		virtual bool MoveDown(NodeBase* &) const = 0;
		virtual bool MoveLeft(NodeBase* &) const = 0;
		virtual bool MoveRight(NodeBase* &) const = 0;
		virtual vector<Key> SubNodeMinKeys() const = 0;
		virtual vector<NodeBase*> SubNodes() const = 0;
		
	protected:
		template <Operation Op>
		static Position ChooseOperatePosition(order_int preCount, order_int thisCount, order_int nxtCount)
		{
			if constexpr (Op == Operation::Add)
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
			else
			{
				return ChooseOperatePosition<Operation::Add>(preCount, thisCount, nxtCount) == Position::Previous ?
					Position::Next : Position::Previous;
			}
		}
	};
}

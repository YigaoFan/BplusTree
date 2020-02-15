#pragma once
/***********************************************************************************************************
 Abstract class NodeBase in Collections
***********************************************************************************************************/

#include <vector>
#include <memory>
#include "Basic.hpp"

namespace Collections
{
	using ::std::move;
	using ::std::unique_ptr;
	using ::std::vector;

	namespace
	{
		enum Position
		{
			Previous,
			Next,
		};

		// Why this enum can not define in LeafNode class? because it has LeafNode::Add adn LeafNode::Remove
		enum Operation
		{
			Add,
			Remove,
		};
	}

	template <typename Key, typename Value, order_int BtreeOrder>
	class NodeBase
	{
	protected:
		static const order_int LowBound = 1 + ((BtreeOrder - 1) / 2);
		function<void(NodeBase*, unique_ptr<NodeBase>)> _upNodeAddSubNodeCallback;
		function<void(NodeBase*)> _upNodeDeleteSubNodeCallback;
		function<void(Key const&, NodeBase*)> _minKeyChangeCallback;// TODO for sibling min change and not split node situation
		NodeBase* _next{ nullptr };
		NodeBase* _previous{ nullptr };
	public:
		void SetUpNodeCallback(function<void(NodeBase*, unique_ptr<NodeBase>)> addSubNodeCallback, 
								function<void(NodeBase*)> deleteSubNodeCallback, 
								function<void(Key const&, NodeBase*)> minKeyChangeCallback)
		{
			_upNodeAddSubNodeCallback = move(addSubNodeCallback);
			_upNodeDeleteSubNodeCallback = move(deleteSubNodeCallback);
			_minKeyChangeCallback = move(minKeyChangeCallback);
		}
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

		NodeBase* Next() const { return _next; }
		NodeBase* Previous() const { return _previous; }
		void Next(NodeBase* next) { _next = next; }
		void Previous(NodeBase* previous) { _previous = previous; }
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

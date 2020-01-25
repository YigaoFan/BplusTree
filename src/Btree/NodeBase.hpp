#pragma once
#include <vector>
#include <memory>
#include <variant>
#include "Basic.hpp"
#include "Util.hpp"
#include "Enumerator.hpp"
#include "Exception.hpp"

namespace Collections
{
	using ::std::move;
	using ::std::unique_ptr;
	using ::std::variant;

	template <typename Key, typename Value, order_int BtreeOrder>
	class NodeBase
	{
	protected:
		function<void(NodeBase*, unique_ptr<NodeBase>)> _upNodeAddSubNodeCallback;
		function<void(NodeBase*)> _upNodeDeleteSubNodeCallback;
	public:
		void SetUpNodeCallback(function<void(NodeBase*, unique_ptr<NodeBase>)> addSubNodeCallback, function<void(NodeBase*)> deleteSubNodeCallback)
		{
			_upNodeAddSubNodeCallback = move(addSubNodeCallback);
			_upNodeDeleteSubNodeCallback = move(deleteSubNodeCallback);
		}
		virtual unique_ptr<NodeBase> Clone() const = 0;
		virtual unique_ptr<NodeBase> Move() const = 0;
		virtual ~NodeBase() = default;
		virtual bool Middle() const = 0;
		virtual vector<Key> Keys() const = 0;
		virtual Key const& MinKey() const = 0;
		virtual bool ContainsKey(Key const&) const = 0;
		virtual Value GetValue(Key const&) const = 0;
		virtual void ModifyValue(Key const&, Value) = 0;
		virtual void Add(pair<Key, Value>) = 0;
		virtual void Remove(Key const&) = 0;
	};
}

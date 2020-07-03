#pragma once
#include <memory>
#include <type_traits>
#include <functional>
#include "Basic.hpp"
#include "Enumerator.hpp"
#include "LeafNode.hpp"
#include "MiddleNode.hpp"
#include "../Basic/TypeTrait.hpp"

namespace Collections
{
	using ::std::move;
	using ::std::shared_ptr;
	using ::std::unique_ptr;
	using ::std::make_unique;
	using ::std::unique_ptr;
	using ::std::remove_reference_t;
	using ::std::function;
	using ::Basic::IsSpecialization;

	template <typename Key, typename Value, order_int BtreeOrder, template <typename...> class Ptr = unique_ptr>
	class NodeFactory
	{
	private:
		using Node = NodeBase<Key, Value, BtreeOrder, Ptr>;
		using Leaf = LeafNode<Key, Value, BtreeOrder, Ptr>;
		using Middle = MiddleNode<Key, Value, BtreeOrder, Ptr>;
		using _LessThan = LessThan<Key>;

	public:
		template <bool LeafCons=true, typename... Ts>
		static unique_ptr<Node> MakeNode(Enumerator<Ts...> enumerator, shared_ptr<_LessThan> lessThan)
		{
			// remove_reference_t sometimes not very good
			if constexpr (IsSpecialization<remove_reference_t<typename Enumerator<Ts...>::ValueType>, unique_ptr>::value)
			{
				return make_unique<Middle>(enumerator, lessThan);
			}
			else
			{
				return make_unique<Leaf>(enumerator, lessThan);
			}
		}

		static void TryShallow(unique_ptr<Node>& root, function<void()> rootChangeCallback)
		{
#define MID_CAST static_cast<Middle *>
			if (root->Middle())
			{
				auto newRoot = MID_CAST(root.get())->HandleOverOnlySon();
				*newRoot = move(*root);
				if (newRoot->Middle())
				{
					auto newMidRoot = MID_CAST(newRoot.get());
					auto midRoot = MID_CAST(root.get());
					newMidRoot->_queryPrevious = midRoot->_queryPrevious;
					newMidRoot->_queryNext = midRoot->_queryPrevious;
				}

				root = move(newRoot);
				rootChangeCallback();
#undef MID_CAST
			}
		}
	};
}
#pragma once
#include <memory>
#include <type_traits>
#include <functional>
#include "Basic.hpp"
#include "Enumerator.hpp"
#include "LeafNode.hpp"
#include "MiddleNode.hpp"
#include "../FuncLib/Store/FileResource.hpp"
#include "../Basic/TypeTrait.hpp"

namespace Collections
{
	using ::Basic::IsSpecialization;
	using ::std::function;
	using ::std::make_unique;
	using ::std::move;
	using ::std::remove_reference_t;
	using ::std::shared_ptr;
	using ::std::unique_ptr;

	template <typename Key, typename Value, order_int BtreeOrder, template <typename...> class Ptr = unique_ptr>
	class NodeFactory
	{
	private:
		using Node = NodeBase<Key, Value, BtreeOrder, Ptr>;
		using Leaf = LeafNode<Key, Value, BtreeOrder, Ptr>;
		using Middle = MiddleNode<Key, Value, BtreeOrder, Ptr>;
		using _LessThan = LessThan<Key>;

	public:
		template <typename... Ts>
		static Ptr<Node> MakeNode(Enumerator<Ts...> enumerator, shared_ptr<_LessThan> lessThan)
		{
			// remove_reference_t sometimes not very good
			using T = remove_reference_t<typename Enumerator<Ts...>::ValueType>;

			if constexpr (IsSpecialization<Ptr<int>, UniqueDiskPtr>::value)
			{
				using FuncLib::Store::FileResource;
				auto f = FileResource::GetCurrentThreadFile().get();
				auto node = Middle(enumerator, lessThan);
				return UniqueDiskPtr<Middle>::MakeUnique(move(node), f);// TODO maybe should handle leaf cons, too
			}
			else
			{
				if constexpr (IsSpecialization<T, unique_ptr>::value)
				{
					return make_unique<Middle>(enumerator, lessThan);
				}
				else
				{
					return make_unique<Leaf>(enumerator, lessThan);
				}
			}
		}

		static void TryShallow(Ptr<Node>& root, function<void()> rootChangeCallback)
		{
#define MID_CAST(NODE) static_cast<typename Node::template OwnerLessPtr<Middle>>(NODE)
			if (root->Middle())
			{
				auto newRoot = MID_CAST(root.get())->HandleOverOnlySon();
				if (newRoot->Middle())
				{
					auto newMidRoot = MID_CAST(newRoot.get());
					auto midRoot = MID_CAST(root.get());
					newMidRoot->_queryPrevious = midRoot->_queryPrevious;
					newMidRoot->_queryNext = midRoot->_queryPrevious;
				}

				root = move(newRoot);
				rootChangeCallback(); // set btree call back here
#undef MID_CAST
			}
		}
	};
}
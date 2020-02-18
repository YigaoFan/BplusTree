#pragma once
#include <memory>
#include <type_traits>
#include <functional>
#include "Basic.hpp"
#include "Enumerator.hpp"
#include "LeafNode.hpp"
#include "MiddleNode.hpp"

namespace Collections
{
	using ::std::move;
	using ::std::shared_ptr;
	using ::std::unique_ptr;
	using ::std::make_unique;
	using ::std::unique_ptr;
	using ::std::remove_reference_t;
	using ::std::function;

	template<typename Test, template<typename...> class Ref>
	struct IsSpecialization : std::false_type {};

	template<template<typename...> class Ref, typename... Args>
	struct IsSpecialization<Ref<Args...>, Ref> : std::true_type {};

	template <typename Key, typename Value, order_int BtreeOrder>
	class NodeFactory
	{
	private:
		using Node = NodeBase<Key, Value, BtreeOrder>;
		using Leaf = LeafNode<Key, Value, BtreeOrder>;
		using Middle = MiddleNode<Key, Value, BtreeOrder>;
		using _LessThan = LessThan<Key>;

	public:
		template <bool LeafCons=true, typename... Ts>
		static unique_ptr<Node> MakeNode(Enumerator<Ts...> enumerator, shared_ptr<_LessThan> lessThan)
		{
			if constexpr (IsSpecialization<remove_reference_t<typename Enumerator<Ts...>::ValueType>, unique_ptr>::value)
			{
				//return make_unique<Middle>(enumerator, lessThan);
				return nullptr;
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
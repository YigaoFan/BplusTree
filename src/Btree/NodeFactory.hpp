#pragma once
#include <memory>
#include <type_traits>
#include <functional>
#include "Basic.hpp"
#include "Enumerator.hpp"
#include "LeafNode.hpp"
#include "MiddleNode.hpp"
#include "../FuncLib/Store/File.hpp"
#include "../Basic/TypeTrait.hpp"

namespace Collections
{
	using Basic::IsSpecialization;
	using FuncLib::Store::File;
	using ::std::function;
	using ::std::make_shared;
	using ::std::make_unique;
	using ::std::move;
	using ::std::remove_reference_t;
	using ::std::unique_ptr;

	template <typename Key, typename Value, order_int BtreeOrder, StorePlace Place>
	class NodeFactory
	{
	private:
		template <typename... Ts>
		using Ptr = typename TypeConfig::template Ptr<Place>::template Type<Ts...>;
		using Node = NodeBase<Key, Value, BtreeOrder, Place>;
		using Leaf = LeafNode<Key, Value, BtreeOrder, Place>;
		using Middle = MiddleNode<Key, Value, BtreeOrder, Place>;
		using _LessThan = LessThan<Key>;

	public:
		template <typename... Ts>
		static Ptr<Node> MakeNodeOnMemory(Enumerator<Ts...> enumerator)
		{
			using T = remove_reference_t<ValueTypeOf<Enumerator<Ts...>>>;

			if constexpr (IsSpecialization<T, unique_ptr>::value)
			{
				return make_unique<Middle>(enumerator);
			}
			else
			{
				return make_unique<Leaf>(enumerator);
			}
		}

		template <typename... Ts>
		static Ptr<Node> MakeNodeOnDisk(File* file, Enumerator<Ts...> enumerator)
		{
			using T = remove_reference_t<ValueTypeOf<Enumerator<Ts...>>>;

			// 使用 make_shared 是希望原位构造，只构造一次，
			// 不调用 move，copy 构造函数，这会破坏原来构造好的对象里的指针有效性
			// 可以利用栈来弄吗，反正关系在栈里是对的
			if constexpr (IsSpecialization<T, UniqueDiskPtr>::value)
			{
				return MakeUnique(make_shared<Middle>(enumerator), file);
			}
			else
			{
				return MakeUnique(make_shared<Leaf>(enumerator), file);
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
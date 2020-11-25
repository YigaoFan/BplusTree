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
		static Ptr<Node> MakeNode(Enumerator<Ts...> enumerator, shared_ptr<_LessThan> lessThan)
		{
			using T = remove_reference_t<ValueTypeOf<Enumerator<Ts...>>>;

			if constexpr (IsDisk<Place>)
			{
				using FuncLib::Store::FileResource;
				// TODO 这里的 f 应该去掉
				auto f = FileResource::GetCurrentThreadFile().get();

				if constexpr (IsSpecialization<T, UniqueDiskPtr>::value)
				{
					auto node = Middle(enumerator, lessThan);
					return UniqueDiskPtr<Middle>::MakeUnique(move(node), f);
				}
				else
				{
					// 要转换 string 到 UniqueDiskRef
					function converter = [f=f](T from)
					{
						// 这里有点把 TypeSelector 的逻辑以这种形式部分重写一遍的意思
						// 转成 NodeBase::StoredKey 和 NodeBase::StoredValue
						return TypeConverter<T, OwnerState::FullOwner>::ConvertFrom(from, f);
					};

					auto converted = EnumeratorPipeline<T, typename decltype(converter)::result_type, decltype(enumerator)>(enumerator, converter);
					auto node = Leaf(converted, lessThan);
					return UniqueDiskPtr<Leaf>::MakeUnique(move(node), f);
				}
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
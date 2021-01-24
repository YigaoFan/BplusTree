#pragma once
#include <type_traits>
#include "../../Basic/Concepts.hpp"
#include "../../Btree/GetNodeType.hpp"

namespace FuncLib::Store
{
	template <typename... Ts>
	struct TypeList;

	template <>
	struct TypeList<>
	{
		static constexpr bool IsNull = true;
	};

	template <typename T, typename... Ts>
	struct TypeList<T, Ts...>
	{
		static constexpr bool IsNull = false;
		using Current = T;
		using Remain = TypeList<Ts...>;
	};

	template <typename T, typename TypeList>
	struct Cons;

	template <typename T, typename... Ts>
	struct Cons<T, TypeList<Ts...>>
	{
		using Result = TypeList<T, Ts...>;
	};

	template <typename T>
	struct GenerateOtherSearchRoutine
	{
		using Result = TypeList<>;
	};

	template <typename T>
	concept ConcreteNodeBase = ::std::is_abstract_v<T> and requires (T& t)
	{
		// 用这个来简单控制是 NodeBase 的类型
		{ t.Middle() } -> Basic::IsSameTo<bool>;
	};

	template <typename T>
	concept ConcreteDeriveNode = (not ::std::is_abstract_v<T>) and requires (T t)
	{
		{ t.Middle() } -> Basic::IsSameTo<bool>;
	};

	using Collections::GetNodeTypeFrom;

	template <typename T>
	requires ConcreteNodeBase<T>
	struct GenerateOtherSearchRoutine<T>
	{
		using Result =
			TypeList<typename GetNodeTypeFrom<T>::Leaf, typename GetNodeTypeFrom<T>::Middle>;
	};

	template <typename T>
	requires ConcreteDeriveNode<T>
	struct GenerateOtherSearchRoutine<T>
	{
		using Result = TypeList<typename GetNodeTypeFrom<T>::Base>;
	};

	template <typename T>
	struct TryGetTakeWithDiskPosDestType
	{
		static constexpr bool IsNull = true;
	};

	using Collections::LeafNode;
	using Collections::MiddleNode;
	using Collections::NodeBase;
	using Collections::order_int;
	using Collections::StorePlace;

	template <typename Key, typename Value, order_int Order, StorePlace Place>
	struct TryGetTakeWithDiskPosDestType<NodeBase<Key, Value, Order, Place>>
	{
		static constexpr bool IsNull = false;
		using Result = NodeBase<Key, Value, Order, Place>;
	};

	template <typename Key, typename Value, order_int Order, StorePlace Place>
	struct TryGetTakeWithDiskPosDestType<MiddleNode<Key, Value, Order, Place>> : public TryGetTakeWithDiskPosDestType<NodeBase<Key, Value, Order, Place>>
	{
	};

	template <typename Key, typename Value, order_int Order, StorePlace Place>
	struct TryGetTakeWithDiskPosDestType<LeafNode<Key, Value, Order, Place>> : public TryGetTakeWithDiskPosDestType<NodeBase<Key, Value, Order, Place>>
	{
	};
}

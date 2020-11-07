#pragma once
#include <type_traits>
#include "../../Basic/Concepts.hpp"
#include "../../Btree/GetNodeType.hpp"

namespace FuncLib::Store
{
	template <typename... Ts>
	struct TypeList;

	template <typename T>
	struct TypeList<T>
	{
		static constexpr bool IsLast = true;
		using Current = T;
	};

	template <typename T, typename... Ts>
	struct TypeList<T, Ts...>
	{
		static constexpr bool IsLast = false;
		using Current = T;
		using Remain = TypeList<Ts...>;
	};

	template <typename T>
	struct GenerateSearchRoutine
	{
		using Result = TypeList<T>;
	};

	template <typename T>
	concept ConcreteNodeBase = ::std::is_abstract_v<T> and requires (T& t)
	{
		// 用这个来简单控制是 NodeBase 的类型
		{ t.Middle() } -> Basic::IsSameTo<bool>;
	};

	using Collections::GetNodeTypeFrom;

	template <typename T>
	requires ConcreteNodeBase<T>
	struct GenerateSearchRoutine<T>
	{
		using Result =
			TypeList<T, typename GetNodeTypeFrom<T>::Leaf, typename GetNodeTypeFrom<T>::Middle>;
	};
}

#pragma once
#include <type_traits>
#include "File.hpp"

namespace FuncLib
{
	using ::std::is_trivial_v;
	using ::std::is_standard_layout_v;

	template <bool Condition, typename A, typename B>
	struct CompileIf;

	template <typename A, typename B>
	struct CompileIf<true, A, B>
	{
		using Type = A;
	};

	template <typename A, typename B>
	struct CompileIf<false, A, B>
	{
		using Type = B;
	};

	template <typename T>
	constexpr bool RawOrNot = is_standard_layout_v<T> && is_trivial_v<T> && sizeof(T) < DiskBlockSize;

	template <typename T>
	using StoredT = typename CompileIf<RawOrNot<T>, T, T>::Type;

	template <typename T, bool = is_standard_layout_v<T> && is_trivial_v<T>>
	struct ByteConverter;
	template <typename T, bool = is_standard_layout_v<T> && is_trivial_v<T>>
	struct TypeConverter;
}
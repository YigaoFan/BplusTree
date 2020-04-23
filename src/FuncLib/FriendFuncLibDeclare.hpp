#pragma once
#include <type_traits>

namespace FuncLib
{
	using ::std::is_trivial_v;
	using ::std::is_standard_layout_v;

	template <typename T, bool = is_standard_layout_v<T> && is_trivial_v<T>>
	struct ByteConverter;
	template <typename T, bool = is_standard_layout_v<T> && is_trivial_v<T>>
	struct TypeConverter;
}
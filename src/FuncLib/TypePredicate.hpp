#pragma once
#include <type_traits>
#include "../Basic/TypeTrait.hpp"

namespace FuncLib
{
	using ::std::is_trivial_v;
	using ::std::is_standard_layout_v;
	using ::Basic::CompileIf;

	template <typename T>
	constexpr bool RawOrNot = is_standard_layout_v<T> && is_trivial_v<T> && sizeof(T) < DiskBlockSize;

	template <typename T>
	using StoredT = typename CompileIf<RawOrNot<T>, T, T>::Type;

	template <typename T, bool = is_standard_layout_v<T> && is_trivial_v<T>>
	struct ByteConverter;
	template <typename T, bool = is_standard_layout_v<T> && is_trivial_v<T>>
	struct TypeConverter;

	struct MemoryScheme
	{
		using Ptr = void;
		template <typename T>
		using Key = void;
		template <typename T>
		using Value = void;
	};

	struct DiskScheme
	{

	};
}
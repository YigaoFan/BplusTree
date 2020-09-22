#pragma once
#include <type_traits>

namespace FuncLib
{
	template <typename T>
	using RR = ::std::remove_reference_t<T>;
	using ::std::is_trivial_v;
	using ::std::is_standard_layout_v;

	template <typename T, bool = is_standard_layout_v<RR<T>> && is_trivial_v<RR<T>>>
	struct ByteConverter;
	template <typename T, bool = is_standard_layout_v<RR<T>> && is_trivial_v<RR<T>>>
	struct TypeConverter;
	// For PtrSetter
	template <typename T>
	class UniqueDiskPtr;
	template <typename T>
	class SharedDiskPtr;
}
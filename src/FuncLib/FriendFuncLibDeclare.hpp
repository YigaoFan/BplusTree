#pragma once
#include <type_traits>

namespace FuncLib
{
	using ::std::is_trivial_v;
	using ::std::is_standard_layout_v;

	template <typename T, bool>
	struct ByteConverter;
	template <typename T, bool>
	struct TypeConverter;
	// For PtrSetter
	template <typename T>
	class DiskPtr;
}
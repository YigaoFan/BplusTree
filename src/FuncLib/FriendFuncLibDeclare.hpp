#pragma once
#include <type_traits>
#include "Switch.hpp"
#include "OwnerState.hpp"

namespace FuncLib
{
	template <typename T>
	using RR = ::std::remove_reference_t<T>;
	using ::std::is_trivial_v;
	using ::std::is_standard_layout_v;

	template <typename T, bool = is_standard_layout_v<RR<T>> && is_trivial_v<RR<T>>>
	struct ByteConverter;
	template <typename T, OwnerState Owner = OwnerState::FullOwner>
	struct TypeConverter;
	// For PtrSetter
	template <typename T>
	class UniqueDiskPtr;
	template <typename T>
	class OwnerLessDiskPtr;

	template <typename T>
	class UniqueDiskRef;
	template <typename T>
	class OwnerLessDiskRef;

	template <typename T>
	class DiskPos;

	template <typename T, Switch SwitchState>
	class TakeWithDiskPos;

	template <typename T>
	class TakeWithDiskPos<T, Switch::Disable>
	{ };
}
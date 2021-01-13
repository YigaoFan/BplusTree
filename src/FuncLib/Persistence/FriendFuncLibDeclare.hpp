#pragma once
#include <type_traits>
#include <memory>
#include "Switch.hpp"
#include "OwnerState.hpp"

namespace FuncLib::Store
{
	class File;
}

namespace FuncLib::Persistence
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

	using ::std::decay_t;
	using ::std::make_shared;
	using Store::File;
	template <typename T1>
	static auto MakeUnique(T1 &&t, File *file) -> UniqueDiskPtr<typename decltype(make_shared<decay_t<T1>>(t))::element_type>;
}
#pragma once

namespace FuncLib
{
	template <typename T>
	DiskPos<T> const& GetDiskPos(UniqueDiskPtr<T> const& ptr)
	{
		return ptr._pos;
	}

	template <typename T>
	DiskPos<T> const& GetDiskPos(UniqueDiskRef<T> const& ref)
	{
		return GetDiskPos(ref._ptr);
	}
}

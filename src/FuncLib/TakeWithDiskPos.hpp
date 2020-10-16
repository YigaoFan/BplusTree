#pragma once
#include "Switch.hpp"

namespace FuncLib
{
	template <typename T, Switch SwitchState>
	class TakeWithDiskPos;

	template <typename T>
	class TakeWithDiskPos<T, Switch::Enable>
	{
	private:
		DiskPos<T> _pos;

	public:
		template <typename TakeWithDiskPosPtr>
		static void SetDiskPos(TakeWithDiskPosPtr ptr, DiskPos<T> pos)
		{
			ptr->_pos = move(pos);
		}

		OwnerLessDiskPtr<T> GetOwnerLessDiskPtr() const
		{
			return { _pos };
		}
	};
}

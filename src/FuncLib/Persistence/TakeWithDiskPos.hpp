#pragma once
#include "Switch.hpp"

namespace FuncLib::Persistence
{
	template <typename T, Switch SwitchState>
	class TakeWithDiskPos;

	template <typename T>
	class TakeWithDiskPos<T, Switch::Enable>
	{
	private:
		DiskPos<T>* _posPtr = nullptr;

	public:
		template <typename TakeWithDiskPosPtr>
		static void SetDiskPos(TakeWithDiskPosPtr ptr, DiskPos<T>* posPtr)
		{
			ptr->_posPtr = posPtr;
		}

		OwnerLessDiskPtr<T> GetOwnerLessDiskPtr() const
		{
			return { *_posPtr };
		}
	};
}

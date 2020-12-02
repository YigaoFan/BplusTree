#pragma once
#include "Switch.hpp"

namespace FuncLib::Persistence
{
	using ::std::move;

	template <typename T, Switch SwitchState>
	class TakeWithDiskPos;

	template <typename T>
	class TakeWithDiskPos<T, Switch::Enable>
	{
	private:
		// 这里去掉指针，是因为一个对象拥有自己的位置是正确的事，如果指向 DiskPtr 里的位置，那 DiskPtr 指向的对象变了呢？
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

		File* GetLessOwnershipFile() const
		{
			return _pos.GetLessOwnershipFile();
		}
	};
}

#include "StorageAllocator.hpp"

namespace FuncLib::Store
{
	StorageAllocator::StorageAllocator(pos_int currentPos, map<pos_lable, pair<pos_int, size_t>> posLableTable)
		: _currentPos(currentPos), _posLableTable(move(posLableTable))
	{ }

	pos_lable StorageAllocator::AllocatePosLable()
	{
		return 0;
	}

	void StorageAllocator::Resize(pos_lable posLable, size_t biggerSize)
	{
		// remove previous position in allocated table
		// return Allocate(newSize);
	}

	StorageAllocator StorageAllocator::ReadAllocatedInfoFrom(path const& filename)
	{
		// TODO
		return { 0, {} };
	}
}

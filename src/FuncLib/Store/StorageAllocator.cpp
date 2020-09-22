#include "StorageAllocator.hpp"
#include "File.hpp"

namespace FuncLib::Store
{
	StorageAllocator::StorageAllocator(pos_int currentPos, vector<shared_ptr<InsidePositionOwner>> ownerTable)
		: _currentPos(currentPos), _ownerTable(ownerTable)
	{ }

	void StorageAllocator::SignIn(shared_ptr<InsidePositionOwner> positionOwner)
	{
		_ownerTable.push_back(positionOwner);
	}

	pos_int StorageAllocator::Allocate(size_t size)
	{
		auto pos = _currentPos;
		_currentPos += size;
		return pos;
	}

	pos_int StorageAllocator::Reallocate(pos_int previousPosition, size_t newSize)
	{
		// remove previous position in allocated table
		return Allocate(newSize);
	}

	StorageAllocator StorageAllocator::ReadAllocatedInfoFrom(path const& filename)
	{
		// TODO
		// SignIn 里的都在这个读到的列表里
		return { 0, {} };
	}
}

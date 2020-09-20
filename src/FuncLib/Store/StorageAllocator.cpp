#include "StorageAllocator.hpp"
#include "File.hpp"

namespace FuncLib::Store
{
	StorageAllocator::StorageAllocator(File* file)
		: _file(file), _currentPos(0), _ownerTable(ReadAllocatedInfoFrom(file))
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
	vector<shared_ptr<InsidePositionOwner>> StorageAllocator::ReadAllocatedInfoFrom(File const* file)
	{
		// TODO
		// SignIn 里的都在这个读到的列表里
		auto path = file->Path();
		return {};
	}
}

#include "StorageAllocator.hpp"
#include "File.hpp"

namespace FuncLib::Store
{
	StorageAllocator::StorageAllocator(File *file)
		: _file(file), _currentPos(0), _ownerTable(ReadAllocatedInfoFrom(file))
	{ }

	void StorageAllocator::SignIn(shared_ptr<IInsidePositionOwner> positionOwner)
	{
		_ownerTable.push_back(positionOwner);
	}

	vector<shared_ptr<IInsidePositionOwner>> StorageAllocator::ReadAllocatedInfoFrom(File const *file)
	{
		// TODO
		// use 
		return {};
	}
}

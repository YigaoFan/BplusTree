#include <memory>
#include <vector>
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using ::std::shared_ptr;
	using ::std::vector;

	class File;

	class StoragePartHandle
	{
	private:
		pos_int* _posPtr;
	public:
		StoragePartHandle(pos_int* posPtr) : _posPtr(posPtr)
		{ }

		pos_int GetConcretePosition()
		{
			return *_posPtr;
		}

		~StoragePartHandle()
		{
			delete _posPtr;
		}
	}

	// 分配信息需要保存到硬盘上
	class StorageAllocator
	{
	private:
		// 这里拿一个字段来存储从硬盘读取的分配信息
		pos_int _currentPos;
		vector<StoragePartHandle> _usedInfoTable;
	public:
		StorageAllocator(/* args */);

		// pos_int Allocate(size_t size)
		// {
		// 	auto pos = _currentPos;
		// 	_currentPos += size;
		// 	return MakePositionOwner<T>(_file, pos);
		// }

		StoragePartHandle Register(pos_int pos)
		{
			auto handle = StoragePartHandle(new pos_int(pos));
			_usedInfoTable.push_back(handle);
			return handle;
		}
	};
}
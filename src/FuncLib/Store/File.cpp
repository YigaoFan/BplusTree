#include "File.hpp"

namespace FuncLib::Store
{
	set<File*> File::Files = {};

	shared_ptr<File> File::GetFile(path const &filename)
	{
		for (auto f : Files)
		{
			if (*(f->_filename) == filename)
			{
				return f->shared_from_this();
			}
		}

		auto f = make_shared<File>(Files.size(), filename);
		Files.insert(f.get());

		return f;
	}

	File::File(unsigned int fileId, path filename)
		: _filename(make_shared<path>(move(filename))), _cache(fileId),
		// 下面这两个应该直接作为参数赋值进来更好 TODO
		 _allocator(StorageAllocator::ReadAllocatedInfoFrom(*_filename)),
		 _objRelationTree(ObjectRelationTree::ReadObjRelationTreeFrom(*_filename))
	{ }

	shared_ptr<path> File::Path() const
	{
		return _filename;
	}

	File::~File()
	{
		_allocator.DeallocatePosLables(_toDeallocateLables);
		StorageAllocator::WriteAllocatedInfoTo(*_filename, _allocator);
		ObjectRelationTree::WriteObjRelationTreeTo(*_filename, _objRelationTree);
		Files.erase(this);
	}
}
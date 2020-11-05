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

		FileReader reader{ make_shared<path>(filename), 0 };
		auto relationTree = ObjectRelationTree::ReadObjRelationTreeFrom(&reader);
		auto allocator = StorageAllocator::ReadAllocatedInfoFrom(&reader);
		auto f = make_shared<File>(Files.size(), filename, move(allocator), move(relationTree));
		Files.insert(f.get());

		return f;
	}

	File::File(unsigned int fileId, path filename, StorageAllocator allocator, ObjectRelationTree relationTree)
		: _filename(make_shared<path>(move(filename))),
		  _cache(fileId), _allocator(move(allocator)),
		  _objRelationTree(move(relationTree))
	{ }

	shared_ptr<path> File::Path() const
	{
		return _filename;
	}

	File::~File()
	{
		_allocator.DeallocatePosLables(_toDeallocateLables);
		ObjectBytes bytes{ 0 };
		StorageAllocator::WriteAllocatedInfoTo(_allocator, &bytes);
		ObjectRelationTree::WriteObjRelationTreeTo(_objRelationTree, &bytes);
		auto pos = 0; // TODO
		ofstream* fs = nullptr;
		bytes.WriteIn(fs, pos);
		Files.erase(this);
	}
}
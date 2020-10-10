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
		 _allocator(StorageAllocator::ReadAllocatedInfoFrom(filename))
	{ }

	shared_ptr<path> File::Path() const
	{
		return _filename;
	}

	File::~File()
	{
		Files.erase(this);
		StorageAllocator::WriteAllocatedInfoTo(*_filename, _allocator);
	}
}
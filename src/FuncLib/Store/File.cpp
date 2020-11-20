#include "File.hpp"

namespace FuncLib::Store
{
	set<File*> File::Files = {};
	constexpr pos_int MetaDataSize = 2048; // Byte
	constexpr pos_int MetaDataStart = 0;

	shared_ptr<File> File::GetFile(path const& filename)
	{
		for (auto f : Files)
		{
			if (*(f->_filename) == filename)
			{
				return f->shared_from_this();
			}
		}

		auto namePtr = make_shared<path>(filename);
		FileReader reader{ namePtr, 0 };
		auto relationTree = ObjectRelationTree::ReadObjRelationTreeFrom(&reader);
		auto allocator = StorageAllocator::ReadAllocatedInfoFrom(&reader);
		auto f = make_shared<File>(Files.size(), namePtr, move(allocator), move(relationTree));
		Files.insert(f.get());

		return f;
	}

	File::File(unsigned int fileId, shared_ptr<path> filename, StorageAllocator allocator, ObjectRelationTree relationTree)
		: _filename(move(filename)),
		  _cache(fileId), _allocator(move(allocator)),
		  _objRelationTree(move(relationTree))
	{ }

	shared_ptr<path> File::Path() const
	{
		return _filename;
	}

	File::~File()
	{
		_allocator.DeallocatePosLabels(_notStoredLabels);
		ObjectBytes bytes{ 0 };
		StorageAllocator::WriteAllocatedInfoTo(_allocator, &bytes);
		_objRelationTree.ReleaseAllFreeNode([&](pos_label label)
		{
			_allocator.DeallocatePosLabel(label);
		});
		ObjectRelationTree::WriteObjRelationTree(_objRelationTree, &bytes);

		if (bytes.Size() > MetaDataSize)
		{
			// throw exception()
			// adjust the size
		}
		else
		{
			ofstream fs = MakeOFileStream(_filename);
			bytes.WriteIn(&fs, MetaDataStart);
		}
		
		Files.erase(this);
	}

	ofstream File::MakeOFileStream(shared_ptr<path> const& filename)
	{
		constexpr ofstream::openmode openmode = ofstream::binary | ofstream::in | ofstream::out;
		return ofstream{ *filename, openmode };
	}
}
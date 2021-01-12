#include "File.hpp"
#include "StoreInfoPersistence.hpp"

namespace FuncLib::Store
{
	constexpr pos_int MetadataSize = 2048; // Byte
	constexpr pos_int MetadataStart = 0;

	shared_ptr<File> File::GetFile(path const& filename)
	{
		for (auto f : Files)
		{
			if (*(f->_filename) == filename)
			{
				return f->shared_from_this();
			}
		}

		if (auto namePtr = make_shared<path>(filename); exists(filename))
		{
			auto reader = FileReader::MakeReader(nullptr, filename, 0);
			auto t = ReadObjRelationTreeFrom(&reader);
			auto a = ReadAllocatedInfoFrom(&reader);
			auto f = make_shared<File>(Files.size(), namePtr, move(a), move(t));
			Files.insert(f.get());
			return f;
		}
		else
		{
			auto t = ObjectRelationTree();
			auto a = StorageAllocator();
			auto f = make_shared<File>(Files.size(), namePtr, move(a), move(t));
			Files.insert(f.get());
			return f;
		}
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
		WriteAllocatedInfoTo(_allocator, &bytes);
		_objRelationTree.ReleaseFreeNodes([this](pos_label label)
		{
			_allocator.DeallocatePosLabel(label);
		});
		WriteObjRelationTree(_objRelationTree, &bytes);

		if (bytes.Size() > MetadataSize)
		{
			// throw exception()
			// adjust the size
		}
		else
		{
			ofstream fs = MakeOFileStream(_filename.get());
			bytes.WriteIn(&fs, MetadataStart);
		}
		
		Files.erase(this);
	}

	// 注意：这个不能创建文件
	ofstream File::MakeOFileStream(path const* filename)
	{
		constexpr ofstream::openmode openmode = ofstream::binary | ofstream::in | ofstream::out;
		return ofstream{ *filename, openmode };
	}
}
#include <fstream>
#include <exception>
#include "File.hpp"
#include "StoreInfoPersistence.hpp"

namespace FuncLib::Store
{
	constexpr pos_int MetadataStart = 0;
	constexpr char const slogan[] = "Hello File";// 留作有效性校验

	shared_ptr<File> File::GetFile(path const& filename)
	{
		if (auto pathPtr = make_shared<path>(filename); exists(filename))
		{
			auto reader = FileReader::MakeReader(nullptr, filename, 0);
			reader.Read<sizeof(slogan)>();
			// 以下这两个东西不允许出现内 disk ptr
			printf("start read allocator info");
			auto a = ReadAllocatedInfoFrom(&reader);
			printf("start read relation tree");
			auto t = ReadObjRelationTreeFrom(&reader);
			printf("read all done");

			auto f = make_shared<File>(FileCount++, pathPtr, move(a), move(t));
			return f;
		}
		else
		{
			auto t = ObjectRelationTree();
			auto a = StorageAllocator();
			auto f = make_shared<File>(FileCount++, pathPtr, move(a), move(t));
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
		bytes.Add(slogan, sizeof(slogan));

		WriteAllocatedInfoTo(_allocator, &bytes);
		_objRelationTree.ReleaseFreeNodes([this](pos_label label)
		{
			_allocator.DeallocatePosLabel(label);
		});
		WriteObjRelationTree(_objRelationTree, &bytes);

		if (bytes.Size() > MetadataSize)
		{
			printf("File metadata size too big, please handle it");
			// throw std::overflow_error("File metadata size too big, please handle it");
			// adjust the size
		}
		else
		{
			printf("byte size %zu\n", bytes.Size());
			auto fs = MakeFileStream(_filename.get());
			// 这里要保证文件肯定存在
			bytes.WriteIn(&fs, MetadataStart);
		}
	}

	fstream File::MakeFileStream(path const* filename)
	{
		// 原位修改
		// 不存在不会创建文件
		constexpr fstream::openmode openmode = fstream::binary | fstream::in | fstream::out;
		return fstream{ *filename };
	}
}
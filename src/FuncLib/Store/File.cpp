#include <fstream>
#include "../Basic/Exception.hpp"
#include "File.hpp"
#include "StoreInfoPersistence.hpp"

namespace FuncLib::Persistence
{
	using FuncLib::Store::File;
	using FuncLib::Store::FileCache;

	template <>
	struct ByteConverter<File, false>
	{
		using ThisType = File;
		using DataMember0 = decltype(declval<ThisType>()._metadataSize);
		// 以下这两个东西不允许出现内 disk ptr
		using DataMember1 = decltype(declval<ThisType>()._allocator);
		using DataMember2 = decltype(declval<ThisType>()._objRelationTree);
		static constexpr bool SizeStable = false;
		// 下面的写和读，顺序要保持一致
		static void WriteDown(ThisType const& p, IWriter auto *writer)
		{
			ByteConverter<DataMember0>::WriteDown(p._metadataSize, writer);
			ByteConverter<DataMember1>::WriteDown(p._allocator, writer);
			WriteObjRelationTree(p._objRelationTree, writer);
		}

		static shared_ptr<ThisType> ReadOut(IReader auto* reader, shared_ptr<path> pathPtr, FileCache cache)
		{
			return make_shared<ThisType>
			(
				ByteConverter<DataMember0>::ReadOut(reader),
				move(cache),
				move(pathPtr),
				ByteConverter<DataMember1>::ReadOut(reader),
				ReadObjRelationTreeFrom(reader)
			);
		}
	};
}

namespace FuncLib::Store
{
	constexpr pos_int MetadataStart = 0;
	constexpr char const slogan[] = "Hello File";// 留作有效性校验

	pos_int GetFitSpaceSize(pos_int dataSize, pos_int currentSpaceSize)
	{
		constexpr pos_int GB = 1024 * 1024 * 1024;
		for (;;)
		{
			currentSpaceSize += 1024;
			if (currentSpaceSize > GB)
			{
				throw std::out_of_range("current require space is too big");
			}

			if (currentSpaceSize > dataSize)
			{
				return currentSpaceSize;
			}
		}
	}

	bool CheckFileValid(array<byte, sizeof(slogan)> const& bytes)
	{
		for (size_t i = 0; i < sizeof(slogan); ++i)
		{
			if (slogan[i] != static_cast<char>(bytes[i]))
			{
				return false;
			}
		}
		return true;
	}

	vector<char> ReadRemainDataFrom(fstream* fs, pos_int pos)
	{
		fs->seekg(pos, fstream::beg);
		auto start = fs->tellg();
		fs->seekg(0, fstream::end);
		auto end = fs->tellg();
		auto size = end - start;
		vector<char> data(size);
		fs->read(reinterpret_cast<char *>(data.data()), size);
		return data;
	}

	shared_ptr<File> File::GetFile(path const& filename)
	{
		if (auto pathPtr = make_shared<path>(filename); exists(filename))
		{
			using Persistence::ByteConverter;

			auto reader = FileReader::MakeReader(nullptr, filename, 0);
			if (not CheckFileValid(reader.Read<sizeof(slogan)>()))
			{
				throw Basic::InvalidOperationException(string(filename) + " is invalid");
			}

			return ByteConverter<File>::ReadOut(&reader, pathPtr, FileCache(FileCount++));
		}
		else
		{
			auto t = ObjectRelationTree();
			auto a = StorageAllocator();
			pos_int metadataSize = 2048;
			auto f = make_shared<File>(metadataSize, FileCache(FileCount++), pathPtr, move(a), move(t));
			return f;
		}
	}

	File::~File()
	{
		_allocator.DeallocatePosLabels(_notStoredLabels);
		_objRelationTree.ReleaseFreeNodes([this](pos_label label)
		{
			_allocator.DeallocatePosLabel(label);
		});

		// Store
		auto fileStoreProcess = [this](ObjectBytes* writer)
		{
			writer->Add(slogan, sizeof(slogan));
			ByteConverter<File>::WriteDown(*this, writer);
		};
		
		ObjectBytes metadataBytes(FileLabel);
		fileStoreProcess(&metadataBytes);
		printf("byte size %zu\n", metadataBytes.Size());

		CreateIfNotExist(_filename.get());
		auto fs = MakeFileStream(_filename.get());
		if (auto size = metadataBytes.Size(); size > _metadataSize) // TODO 测一下
		{
			printf("File metadata size too big, adjust space");
			auto data = ReadRemainDataFrom(&fs, _metadataSize);
			ObjectBytes dataBytes(FileLabel, move(data));

			_metadataSize = GetFitSpaceSize(size, _metadataSize);
			ObjectBytes newMetadataBytes(FileLabel);
			newMetadataBytes.WriteIn(&fs, MetadataStart);
			dataBytes.WriteIn(&fs, _metadataSize);
		}
		else
		{
			metadataBytes.WriteIn(&fs, MetadataStart);
		}
	}

	File::File(pos_int metadataSize, FileCache cache, shared_ptr<path> filename, StorageAllocator allocator, ObjectRelationTree relationTree)
		: _metadataSize(metadataSize), _filename(move(filename)), _cache(move(cache)), _allocator(move(allocator)),
		  _objRelationTree(move(relationTree))
	{ }

	fstream File::MakeFileStream(path const* filename)
	{
		// 原位修改
		// 不存在不会创建文件
		constexpr fstream::openmode openmode = fstream::binary | fstream::in | fstream::out;
		return fstream{ *filename };
	}

	void File::CreateIfNotExist(path const* filename)
	{
		using ::std::ofstream;
		using ::std::filesystem::exists;

		if (not exists(*filename))
		{
			ofstream f(*filename);
		}
	}
}
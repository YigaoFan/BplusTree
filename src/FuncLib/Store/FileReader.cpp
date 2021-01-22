#include "FileReader.hpp"

namespace FuncLib::Store
{
	using ::std::ifstream;

	vector<byte> ReadByte(ifstream* readStream, pos_int start, size_t size)
	{
		if (size == 0)
		{
			return { };
		}

		// fs.unsetf(ifstream::skipws); // Stop eating new lines in binary mode
		readStream->seekg(start);
		vector<byte> mem(size);
		readStream->read(reinterpret_cast<char*>(mem.data()), size);

		return mem;
	}

	FileReader FileReader::MakeReader(File* file, path const& filename, pos_int pos)
	{
		ifstream fs(filename, ifstream::in | ifstream::binary);
		return FileReader(file, move(fs), pos);
	}

	FileReader::FileReader(ifstream readStream, pos_int startPos)
		: FileReader(nullptr, move(readStream), startPos)
	{ }

	FileReader::FileReader(File *file, ifstream readStream, pos_int startPos)
		: _file(file), _readStream(move(readStream)), _pos(startPos)
	{ }

	vector<byte> FileReader::Read(size_t size)
	{
		auto pos = _pos;
		_pos += size;
		return ReadByte(&_readStream, pos, size);
	}

	void FileReader::Skip(size_t size)
	{
		Read(size);
	}

	File* FileReader::GetLessOwnershipFile() const
	{
		return _file;
	}
}

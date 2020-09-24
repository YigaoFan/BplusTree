#include "FileReader.hpp"

namespace FuncLib::Store
{
	vector<byte> ReadByte(path const& filename, pos_int start, size_t size)
	{
		if (size == 0)
		{
			return { };
		}

		ifstream fs(filename, ifstream::in | ifstream::binary);
		fs.unsetf(ifstream::skipws);
		fs.seekg(start);

		vector<byte> mem(size);

		if (fs.is_open())
		{
			fs.read(reinterpret_cast<char *>(&mem[0]), size);
		}

		return mem;
	}

	FileReader::FileReader(shared_ptr<path> filename, pos_int startPos)
		: FileReader(nullptr, move(filename), startPos)
	{ }

	FileReader::FileReader(File* file, shared_ptr<path> filename, pos_int startPos)
		: _file(file), _filename(filename), _pos(startPos)
	{ }

	vector<byte> FileReader::Read(size_t size)
	{
		auto pos = _pos;
		_pos += size;
		return ReadByte(*_filename, pos, size);
	}

	File* FileReader::GetLessOwnershipFile() const
	{
		return _file;
	}
}

#include "FileReader.hpp"
#include "File.hpp"

namespace FuncLib::Store
{
	vector<byte> ReadByte(path const& filename, pos_int start, size_t size)
	{
		ifstream fs(filename, ifstream::binary);
		fs.seekg(start);

		vector<byte> mem(size);

		if (fs.is_open())
		{
			fs.read(reinterpret_cast<char *>(&mem[0]), size);
		}

		return mem;
	}

	FileReader::FileReader(File* file, pos_int startPos) : _file(file), _pos(startPos)
	{ }

	vector<byte> FileReader::Read(size_t size)
	{
		auto pos = _pos;
		_pos += size;
		auto path = GetPath();
		return ReadByte(*path, pos, size);
	}

	File* FileReader::GetLessOwnershipFile() const
	{
		return _file;
	}

	shared_ptr<path> FileReader::GetPath() const
	{
		return _file->Path();
	}
}

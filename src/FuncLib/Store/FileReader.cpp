#include "FileReader.hpp"

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

	vector<byte> FileReader::Read(size_t size)
	{
		auto pos = pos_;
		pos_ += size;
		auto path = GetPath();
		return ReadByte(*path, pos, size);
	}
}

#include "FileReader.hpp"

namespace FuncLib::Store
{
	vector<byte> Read(path const& filename, pos_int start, size_t size)
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
		auto p = pos_;
		pos_ += size;
		return Read(*filename_, pos_, size);
	}
}

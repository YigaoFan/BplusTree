#include <fstream>
#include "FileWriter.hpp"

namespace FuncLib::Store
{
	using ::std::ofstream;

	void WriteByte(path const& filename, pos_int start, char const* begin, size_t size)
	{
		ofstream fs(filename, ofstream::binary);
		fs.seekp(start);
		fs.write(begin, size);
	}

	void FileWriter::Write(char const* begin, size_t size)
	{
		auto pos = pos_;
		pos_ += size;

		auto path = GetPath();
		WriteByte(*path, pos, begin, size);
	}
}

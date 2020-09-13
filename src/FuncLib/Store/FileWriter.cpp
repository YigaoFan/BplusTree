#include <fstream>
#include "FileWriter.hpp"

namespace FuncLib::Store
{
	using ::std::ofstream;

	void WriteByte(path const& filename, pos_int start, char const* begin, char const* end)
	{
		ofstream fs(filename, ofstream::binary);
		fs.seekp(start);
		fs.write(begin, end - begin);
	}

	void FileWriter::Write(char const* begin, char const* end)
	{
		auto pos = pos_;
		pos_ += (end - begin);

		auto path = GetPath();
		WriteByte(*path, pos, begin, end);
	}
}

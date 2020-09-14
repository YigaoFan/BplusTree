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

	FileWriter::FileWriter(shared_ptr<path> filename, pos_int startPos)
		: _filename(move(filename)), _pos(startPos)
	{ }

	void FileWriter::Write(char const* begin, size_t size)
	{
		auto pos = _pos;
		_pos += size;

		WriteByte(*_filename, pos, begin, size);
	}
}

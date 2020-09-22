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

	FileWriter::FileWriter(shared_ptr<path> filename) : FileWriter(filename, 0)
	{ }

	// void FileWriter::CurrentPos(pos_int pos)
	// {
	// 	_pos = pos;
	// }

	pos_int FileWriter::CurrentPos() const
	{
		return _pos;
	}

	void FileWriter::Write(char const* begin, size_t size)
	{
		auto pos = _pos;
		_pos += size;

		WriteByte(*_filename, pos, begin, size);
	}

	// TODO
	// size_t FileWriter::BufferSize() const
	// {
	// 	return 0;
	// }

	void FileWriter::Flush()
	{

	}

	void FileWriter::WriteBlank(size_t count)
	{

	}

	FileWriter::~FileWriter()
	{
		Flush();
	}
}

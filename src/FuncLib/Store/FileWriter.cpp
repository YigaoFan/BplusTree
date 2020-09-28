#include <fstream>
#include "FileWriter.hpp"

namespace FuncLib::Store
{
	using ::std::ofstream;
	using ::std::filesystem::exists;

	void WriteByte(path const& filename, pos_int start, char const* begin, size_t size)
	{
		if (!exists(filename))
		{
			// create file
			ofstream f(filename);
		}

		ofstream fs(filename, ofstream::binary | ofstream::in | ofstream::out);
		fs.seekp(start);
		fs.write(begin, size);
	}

	void DuplicateWriteByte(path const &filename, pos_int start, size_t count, char ch)
	{
		if (!exists(filename))
		{
			// create file
			ofstream f(filename);
		}

		ofstream fs(filename, ofstream::binary | ofstream::in | ofstream::out);
		fs.seekp(start);
		while (count > 0)
		{
			fs.put(ch);
			--count;
		}
	}

	FileWriter::FileWriter(shared_ptr<path> filename, pos_int startPos)
		: _filename(move(filename)), _pos(startPos)
	{ }

#define RUN_COUNTER      \
	if (_counterRunning) \
	{                    \
		++_counter;      \
	}

	void FileWriter::Write(char const* begin, size_t size)
	{
		RUN_COUNTER;
		auto pos = _pos;
		_pos += size;

		WriteByte(*_filename, pos, begin, size);
	}

	void FileWriter::WriteBlank(size_t count)
	{
		RUN_COUNTER;
		auto pos = _pos;
		_pos += count;

		DuplicateWriteByte(*_filename, pos, count, ' ');
	}

	void FileWriter::StartCounter()
	{
		_counterRunning = true;
	}

	void FileWriter::EndCounter()
	{
		_counterRunning = false;
	}

	pos_int FileWriter::CounterNum() const
	{
		return _counter;
	}

	void FileWriter::Flush()
	{

	}

	FileWriter::~FileWriter()
	{
		Flush();
	}
}

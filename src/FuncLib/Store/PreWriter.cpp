#include "PreWriter.hpp"

namespace FuncLib::Store
{
	PreWriter::PreWriter()
	{

	}

	size_t PreWriter::Size() const
	{
		return _buffer.size();
	}

	void PreWriter::StartPos(pos_int pos)
	{
		_startPos = pos;
	}

#define RUN_COUNTER      \
	if (_counterRunning) \
	{                    \
		++_counter;      \
	}

	void PreWriter::Write(char const* begin, size_t size)
	{
		RUN_COUNTER;
		_buffer.insert(end(_buffer), begin, begin + size);
	}

	void PreWriter::WriteBlank(size_t size)
	{
		RUN_COUNTER;
		_buffer.insert(end(_buffer), size, ' ');
	}
#undef RUN_COUNTER

	void PreWriter::StartCounter()
	{
		_counterRunning = true;
	}

	void PreWriter::EndCounter()
	{
		_counterRunning = false;
	}

	pos_int PreWriter::CounterNum() const
	{
		return _counter;
	}

	void PreWriter::Flush()
	{
		// TODO
	}

	PreWriter::~PreWriter()
	{
		Flush();
	}
}

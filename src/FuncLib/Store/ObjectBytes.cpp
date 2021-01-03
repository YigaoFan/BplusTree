#include "ObjectBytes.hpp"

namespace FuncLib::Store
{
	using ::std::move;

	void WriteByte(ofstream* fs, pos_int start, char const* begin, size_t size)
	{
		fs->seekp(start);
		fs->write(begin, size);
	}

	ObjectBytes::ObjectBytes(pos_label label, WriteQueue* writeQueue, AllocateSpaceQueue* allocateQueue, ResizeSpaceQueue* resizeQueue)
		: Base(label), ToWrites(writeQueue), ToAllocates(allocateQueue), ToResizes(resizeQueue)
	{ }

	void ObjectBytes::WriteIn(ofstream* fileStream, pos_int pos) const
	{
		DoWrite(fileStream, pos);
	}

	size_t ObjectBytes::Size() const
	{
		return _bytes.size();
	}
	
	void ObjectBytes::Add(char const* begin, size_t size)
	{
		for (size_t i = 0; i < size; ++i)
		{
			_bytes.push_back(begin[i]);
		}
	}

	void ObjectBytes::AddBlank(size_t size)
	{
		for (size_t i = 0; i < size; ++i)
		{
			_bytes.push_back(Blank);
		}
	}

	void ObjectBytes::DoWrite(ofstream* stream, pos_int& pos) const
	{
		auto size = _bytes.size();

		if (not _bytes.empty())
		{
			WriteByte(stream, pos, &_bytes[0], size);
		}
	}

	bool ObjectBytes::Written() const
	{
		return not _bytes.empty();
	}
}

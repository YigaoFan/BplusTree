#include "ObjectBytes.hpp"

namespace FuncLib::Store
{

	void WriteByte(ofstream* fs, pos_int start, char const* begin, size_t size)
	{
		fs->seekp(start);
		fs->write(begin, size);
	}

	ObjectBytes::ObjectBytes(pos_lable lable, WriteQueue* writeQueue, AllocateSpaceQueue* allocateQueue, ResizeSpaceQueue* resizeQueue)
		: _lable(lable), ToWrites(writeQueue), ToAllocates(allocateQueue), ToResizes(resizeQueue)
	{ }

	void ObjectBytes::WriteIn(ofstream* fileStream, pos_int pos) const
	{
		DoWrite(fileStream, pos);
	}

	void ObjectBytes::AddSub(ObjectBytes* subObjectBytes)
	{
		_subObjectBytes.push_back(subObjectBytes);
	}

	pos_lable ObjectBytes::Lable() const
	{
		return _lable;
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
			_bytes.push_back(' ');
		}
	}

	void ObjectBytes::DoWrite(ofstream* stream, pos_int& pos) const
	{
		auto size = _bytes.size();

		if (not _bytes.empty())
		{
			WriteByte(stream, pos, &_bytes[0], size);
		}

		// pos += size;

		// for (auto o : _subObjectBytes)
		// {
		// 	o->DoWrite(stream, pos);
		// }
	}
}

#include "ObjectBytes.hpp"

namespace FuncLib::Store
{
	using ::std::move;

	void WriteByte(fstream* fs, pos_int start, char const* begin, size_t size)
	{
		fs->seekp(start, std::fstream::beg);
		fs->write(begin, size);
	}

	ObjectBytes::ObjectBytes(pos_label label, WriteQueue* writeQueue, AllocateSpaceQueue* allocateQueue, ResizeSpaceQueue* resizeQueue)
		: Base(label), ToWrites(writeQueue), ToAllocates(allocateQueue), ToResizes(resizeQueue)
	{ }

	void ObjectBytes::WriteIn(fstream* fileStream, pos_int pos) const
	{
		if (not _bytes.empty())
		{
			auto size = _bytes.size();
			printf("write %d start: %lu size: %lu \n", _label, pos, size);
			WriteByte(fileStream, pos, _bytes.data(), size);
		}
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

	bool ObjectBytes::Written() const
	{
		return not _bytes.empty();
	}

	ObjectBytes* ObjectBytes::ConstructSub(pos_label label)
	{
		auto sub = Base::ConstructSub(label);
		sub->ToWrites = ToWrites;
		sub->ToAllocates = ToAllocates;
		sub->ToResizes = ToResizes;
		return sub;
	}
}

#include "ObjectBytes.hpp"

namespace FuncLib::Store
{
	using ::std::filesystem::exists;

	constexpr ofstream::openmode Openmode = ofstream::binary | ofstream::in | ofstream::out;

	void WriteByte(ofstream* fs, pos_int start, char const* begin, size_t size)
	{
		fs->seekp(start);
		fs->write(begin, size);
	}

	ObjectBytes::ObjectBytes(pos_lable lable, WriteQueue* writeQueue, AllocateSpaceQueue* allocateQueue, ResizeSpaceQueue* resizeQueue)
		: _lable(lable), ToWrites(writeQueue), ToAllocates(allocateQueue), ToResizes(resizeQueue)
	{ }

	void ObjectBytes::WriteIn(path const& filename, pos_int pos) const
	{
		// create file if not exists
		if (!exists(filename)) { ofstream f(filename); }

		ofstream fs(filename, Openmode);
		DoWrite(&fs, pos);
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
		auto size = _bytes.size();

		// for (auto s : _subObjectBytes)
		// {
		// 	size += s->Size();
		// }

		return size;
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

		pos += size;

		for (auto o : _subObjectBytes)
		{
			o->DoWrite(stream, pos);
		}
	}
}

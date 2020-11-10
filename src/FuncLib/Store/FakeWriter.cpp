#include "FakeWriter.hpp"

namespace FuncLib::Store
{
	using ::std::move;
	
	FakeWriter::FakeWriter(pos_lable lable) : _lable(lable)
	{ }

	void FakeWriter::Add(char const* begin, size_t size)
	{ }

	void FakeWriter::AddBlank(size_t size)
	{ }

	void FakeWriter::AddSub(FakeWriter subWriter)
	{
		_subWriters.push_back(move(subWriter));
	}
		
	pos_lable FakeWriter::Lable() const
	{
		return _lable;
	}
}
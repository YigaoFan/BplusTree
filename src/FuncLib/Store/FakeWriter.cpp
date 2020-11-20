#include "FakeWriter.hpp"

namespace FuncLib::Store
{
	using ::std::move;
	
	FakeWriter::FakeWriter(pos_label label) : _label(label)
	{ }

	void FakeWriter::Add(char const* begin, size_t size)
	{ }

	void FakeWriter::AddBlank(size_t size)
	{ }

	void FakeWriter::AddSub(FakeWriter subWriter)
	{
		_subWriters.push_back(move(subWriter));
	}
		
	pos_label FakeWriter::Label() const
	{
		return _label;
	}
}
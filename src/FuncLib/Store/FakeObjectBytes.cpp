#include "FakeObjectBytes.hpp"

namespace FuncLib::Store
{
	void FakeObjectBytes::Add(char const* begin, size_t size)
	{
		_written = true;
	}

	void FakeObjectBytes::AddBlank(size_t size)
	{
		_written = true;
	}

	bool FakeObjectBytes::Written() const 
	{
		return _written;
	}
}
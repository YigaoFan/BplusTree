#include "FakeObjectBytes.hpp"

namespace FuncLib::Store
{	
	FakeObjectBytes::FakeObjectBytes(pos_label label) : Base(label)
	{ }

	void FakeObjectBytes::Add(char const* begin, size_t size)
	{ }

	void FakeObjectBytes::AddBlank(size_t size)
	{ }
}
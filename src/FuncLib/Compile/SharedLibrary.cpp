#include "SharedLibrary.hpp"

namespace FuncLib::Compile
{
	SharedLibrary::SharedLibrary(char const* filename) : _handle(dlopen(filename, RTLD_LAZY))
	{
		if (_handle == nullptr)
		{
			throw InvalidOperationException(string("open ") + filename + " failed");
		}
	}

	SharedLibrary::SharedLibrary(string const &filename) : SharedLibrary(filename.c_str())
	{ }

	SharedLibrary::SharedLibrary(SharedLibrary&& that) noexcept
		: _handle(that._handle)
	{
		that._handle = nullptr;
	}

	SharedLibrary::~SharedLibrary() noexcept(false)
	{
		if (_handle != nullptr)
		{
			if (dlclose(_handle) != 0)
			{
				throw InvalidOperationException("close handle failed");
			}
		}
	}
}

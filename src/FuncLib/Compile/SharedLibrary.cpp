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

	SharedLibrary::~SharedLibrary()
	{
		if (_handle != nullptr)
		{
			dlclose(_handle);
			// 这里不做错误处理，两点原因：
			// 1.在 handle 没错的情况下，极小概率出错
			// 2.出错了本程序也处理不了，错了对本程序没有影响
			// 想错误处理请显式使用 Close 函数
		}
	}

	void SharedLibrary::Close()
	{
		if (_handle != nullptr)
		{
			if (dlclose(_handle) != 0)
			{
				throw InvalidOperationException("close handle failed");
			}
			_handle = nullptr;
		}
	}
}

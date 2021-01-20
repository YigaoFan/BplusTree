#pragma once
#include <dlfcn.h>
#include <string>
#include "../Basic/Exception.hpp"

namespace FuncLib::Compile
{
	using Basic::InvalidOperationException;
	using ::std::forward;
	using ::std::string;

	class SharedLibrary
	{
	private:
		void* _handle;

	public:
		SharedLibrary(char const* filename) : _handle(dlopen(filename, RTLD_LAZY))
		{
			if (_handle == nullptr)
			{
				throw InvalidOperationException(string("open ") + filename + " failed");
			}
		}

		SharedLibrary(string const& filename) : SharedLibrary(filename.c_str())
		{ }

		SharedLibrary(SharedLibrary&& that) noexcept
			: _handle(that._handle)
		{
			that._handle = nullptr;
		}

		SharedLibrary(SharedLibrary const& that) noexcept = delete;

		// 返回值 decltype(auto) 是我在 CompileProcess 使用后感觉
		template <typename Func, typename... Args>
		decltype(auto) Invoke(char const* funcName, Args&&... args)
		{
			auto func = reinterpret_cast<Func*>(dlsym(_handle, funcName));
			if (func == nullptr)
			{
				auto error = dlerror();
				throw InvalidOperationException(string("load ") + funcName + " failed: " + error);
			}

			return func(forward<Args>(args)...);
		}

		template <typename Func, typename... Args>
		decltype(auto) Invoke(string const& funcName, Args&&... args)
		{
			return Invoke(funcName.c_str(), forward<Args>(args)...);
		}

		~SharedLibrary() noexcept(false)
		{
			if (_handle != nullptr)
			{
				if (dlclose(_handle) != 0)
				{
					throw InvalidOperationException("close handle failed");
				}
			}
		}
	};
}

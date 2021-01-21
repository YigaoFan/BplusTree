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
		SharedLibrary(char const* filename);
		SharedLibrary(string const& filename);
		SharedLibrary(SharedLibrary&& that) noexcept;
		SharedLibrary(SharedLibrary const& that) noexcept = delete;
		~SharedLibrary() noexcept(false);

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
	};
}

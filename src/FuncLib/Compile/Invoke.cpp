#include "Invoke.hpp"
#include <string>
#include <dlfcn.h>
#include <fstream>
#include <filesystem>
#include <dlfcn.h>
#include "../../Basic/Exception.hpp"

namespace FuncLib::Compile
{
	using Basic::InvalidOperationException;
	using ::std::move;
	using ::std::ofstream;
	using ::std::string;
	using ::std::filesystem::remove;

	/// Invoke on *nix OS
	JsonObject Invoke(vector<char> const& bin, char const* name, JsonObject args)
	{
		char const* tempFileName = "temp_invoke.so";// 改名
		ofstream of(tempFileName);
		for (auto b : bin)
		{
			of.put(b);
		}

		void* handle = dlopen(tempFileName, RTLD_LAZY);
		if (handle == nullptr)
		{
			throw InvalidOperationException(string("open ") + tempFileName + " failed");
		}

		struct Cleaner
		{
			void* Handle;
			char const* ToDeleteFileName;
			~Cleaner() noexcept(false)
			{
				auto flag = dlclose(Handle);
				remove(ToDeleteFileName);
				if (flag < 0)
				{
					throw InvalidOperationException(string("close handle failed"));
				}
			}
		};
		Cleaner releaser{ handle, tempFileName };
		using InvokeFunc = JsonObject (*)(JsonObject);
		InvokeFunc func = reinterpret_cast<InvokeFunc>(dlsym(handle, name));
		char* error;
		if ((error = dlerror()) != NULL)
		{
			throw InvalidOperationException(string("load ") + name + " failed: " + error);
		}
		
		return func(move(args));
	}
}

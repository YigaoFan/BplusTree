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

	void NewSoFile(char const* filename, vector<char> const& bytes)
	{
		ofstream of(filename);
		for (auto b : bytes)
		{
			of.put(b);
		}
	}

	/// Invoke on *nix OS
	JsonObject Invoke(vector<char> const& bin, char const* funcName, JsonObject args)
	{
		char const* tempFileName = "temp_invoke.so";
		NewSoFile(tempFileName, bin);

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
		Cleaner cleaner{ handle, tempFileName };
		using InvokeFunc = JsonObject (*)(JsonObject);
		InvokeFunc func = reinterpret_cast<InvokeFunc>(dlsym(handle, funcName));
		char* error;
		if ((error = dlerror()) != NULL)
		{
			throw InvalidOperationException(string("load ") + funcName + " failed: " + error);
		}
		
		return func(move(args));
	}
}

#include "Invoke.hpp"
#include <string>
#include <dlfcn.h>
#include <fstream>
#include <filesystem>
#include <dlfcn.h>
#include "Util.hpp"
#include "../../Basic/Exception.hpp"

namespace FuncLib::Compile
{
	using Basic::InvalidOperationException;
	using ::std::move;
	using ::std::ofstream;
	using ::std::string;
	using ::std::filesystem::remove;

	void NewSoFile(string const& filename, vector<char> const& bytes)
	{
		ofstream of(filename);
		of.write(bytes.data(), bytes.size());
	}

	struct Cleaner
	{
		void *Handle;
		string const &ToDeleteFileName;
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

	/// Invoke on *nix OS
	JsonObject Invoke(vector<char> const& bin, char const* funcName, JsonObject args)
	{
		string tempFileName = "temp_invoke" + RandomString() + ".so";
		NewSoFile(tempFileName, bin);

		// 本项目因为需要动态链接，需要特殊编译吗？ TODO
		void* handle = dlopen(tempFileName.c_str(), RTLD_LAZY);
		if (handle == nullptr)
		{
			throw InvalidOperationException(string("open ") + tempFileName + " failed");
		}
		
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

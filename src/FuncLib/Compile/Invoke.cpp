#include <string>
#include <fstream>
#include "Invoke.hpp"
#include "SharedLibrary.hpp"
#include "Util.hpp"

namespace FuncLib::Compile
{
	using ::std::move;
	using ::std::ofstream;
	using ::std::string;

	JsonObject Invoke(vector<char> const& bin, char const* funcName, JsonObject arg)
	{
		string tempFileName = "temp_invoke" + RandomString() + ".so";
		ofstream of(tempFileName);
		of.write(bin.data(), bin.size());
		FilesCleaner cleaner(tempFileName);

		SharedLibrary lib(tempFileName);
		return lib.Invoke<JsonObject(JsonObject)>(funcName, move(arg));
	}
}

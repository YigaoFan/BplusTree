#include <string>
#include <filesystem>

namespace FuncLib::Test
{
	using ::std::string;
	using ::std::filesystem::path;

	path MakeFilePath(string filename);
}
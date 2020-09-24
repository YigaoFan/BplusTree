#include <string>
#include <memory>
#include <filesystem>

namespace FuncLib::Test
{
	using ::std::shared_ptr;
	using ::std::string;
	using ::std::filesystem::path;

	shared_ptr<path> MakeFilePath(string filename);
}
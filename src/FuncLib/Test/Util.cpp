#include "Util.hpp"

namespace FuncLib::Test
{
	path MakeFilePath(string filename)
	{
		return path(filename);
	}

	Cleaner::Cleaner(char const *path) : Path(path) {}

	Cleaner::~Cleaner()
	{
		remove(Path);
	}
}
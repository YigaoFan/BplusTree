#include "Util.hpp"

namespace FuncLib::Test
{
	using ::std::make_shared;

	shared_ptr<path> MakeFilePath(string filename)
	{
		return make_shared<path>(filename);
	}
}
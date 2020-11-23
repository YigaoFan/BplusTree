#pragma once
#include <vector>
#include <string>
#include "FuncType.hpp"

namespace FuncLib::Compile
{
	using ::std::string;
	using ::std::vector;

	struct FuncObj
	{
		FuncType Type;
		string Summary;
	};
}

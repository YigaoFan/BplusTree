#pragma once
#include <string>
#include <vector>
#include "FuncType.hpp"

namespace FuncLib::Compile
{
	using ::std::string;
	using ::std::vector;

	struct FuncObj
	{
		FuncType Type;
		vector<string> ParaNames;
		string Summary;
	};
}

#pragma once
#include <string>
#include "FuncType.hpp"

namespace FuncLib::Compile
{
	using ::std::string;

	struct FuncObj
	{
		FuncType Type;
		string Summary;
	};
}

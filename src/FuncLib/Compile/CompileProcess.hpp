#pragma once
#include <utility>
#include <vector>
#include "FuncObj.hpp"
#include "FuncsDefReader.hpp"

namespace FuncLib::Compile
{
	using ::std::pair;
	using ::std::vector;

	pair<vector<FuncObj>, vector<char>> Compile(FuncsDefReader defReader);
}
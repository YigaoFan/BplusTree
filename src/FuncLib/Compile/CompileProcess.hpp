#pragma once
#include <utility>
#include <vector>
#include <string>
#include <string_view>
#include "FuncObj.hpp"
#include "FuncsDefReader.hpp"

namespace FuncLib::Compile
{
	using ::std::pair;
	using ::std::string;
	using ::std::string_view;
	using ::std::vector;

	pair<vector<FuncObj>, vector<char>> Compile(FuncsDefReader defReader);
	string GetWrapperFuncName(string_view rawName);
}
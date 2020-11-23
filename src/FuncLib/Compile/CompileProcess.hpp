#pragma once
#include <utility>
#include <vector>
#include "FuncObj.hpp"
#include "FuncDefTokenReader.hpp"

namespace FuncLib::Compile
{
	using ::std::pair;
	using ::std::vector;

	/// include 所有必要的头文件，让外界方便使用，后期需求
	pair<vector<FuncObj>, vector<char>> Compile(FuncDefTokenReader* defReader);
}
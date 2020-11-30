#include <vector>
#include <utility>
#include <string>
#include <array>
#include "FuncDefTokenReader.hpp"
#include "FuncType.hpp"

namespace FuncLib::Compile
{
	using ::std::array;
	using ::std::pair;
	using ::std::vector;

	/// 不支持全局变量
	/// 不支持模板，以及非 JSON 包含的基本类型作为参数和返回值
	/// pair: FuncType, func body
	vector<pair<FuncType, vector<string>>> ParseFunc(FuncDefTokenReader* defReader);
}

#include <vector>
#include <utility>
#include <string>
#include <array>
#include "FuncDefTokenReader.hpp"

namespace FuncLib
{
	using ::std::array;
	using ::std::pair;
	using ::std::vector;

	/// 不支持全局变量
	/// 不支持模板，以及非 JSON 包含的基本类型作为参数和返回值
	/// 使用前要保证函数编译正确，比如可以先编译一遍
	/// pair: { return type, name, args }, func body 
	vector<pair<array<string, 3>, vector<string>>> ParseFunc(FuncDefTokenReader* defReader);
}

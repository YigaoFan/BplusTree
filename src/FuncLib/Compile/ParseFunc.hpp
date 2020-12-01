#include <vector>
#include <string>
#include <tuple>
#include "FuncDefTokenReader.hpp"
#include "FuncType.hpp"

namespace FuncLib::Compile
{
	using ::std::array;
	using ::std::tuple;
	using ::std::vector;

	/// 不支持全局变量
	/// 不支持模板，以及非 JSON 包含的基本类型作为参数和返回值
	/// tuple: FuncType, para name, func body
	vector<tuple<FuncType, vector<string>, vector<string>>> ParseFunc(FuncDefTokenReader* defReader);
}

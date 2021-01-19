#include <vector>
#include <string>
#include <tuple>
#include <string_view>
#include "FuncDefTokenReader.hpp"
#include "FuncType.hpp"

#include <utility> // 可能是暂时的
namespace FuncLib::Compile
{
	using ::std::array;
	using ::std::tuple;
	using ::std::vector;
	using ::std::string_view;

	using ::std::pair;

	/// 不支持全局变量
	/// 不支持模板，以及非 JSON 包含的基本类型作为参数和返回值，比如参数类型不支持指针类型
	/// tuple: FuncType, para name, func body
	vector<tuple<FuncType, vector<string>, vector<string>>> ParseFunc(string_view code);

	pair<bool, size_t> CheckBracesBalance(string_view s, size_t& i);
}

#include <vector>
#include <string>
#include <tuple>
#include <utility>
#include <string_view>
#include "FuncType.hpp"
namespace FuncLib::Compile
{
	using ::std::function;
	using ::std::pair;
	using ::std::string_view;
	using ::std::tuple;
	using ::std::vector;

	/// 前提是代码已经通过编译的检查
	/// 不支持全局变量
	/// 不支持模板，以及非 JSON 包含的基本类型作为参数和返回值，比如参数类型不支持指针类型
	/// 包含一点对函数体内容的检测
	/// tuple: FuncType, para name
	vector<tuple<FuncType, vector<string>>> ParseFunc(string_view code);
	pair<bool, size_t> CheckBracesBalance(string_view s, size_t& i);
}

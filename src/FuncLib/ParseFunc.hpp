#include <vector>
#include <utility>
#include <string_view>
#include <string>
#include <array>
#include "FuncDefTokenReader.hpp"

namespace FuncLib
{
	using ::std::array;
	using ::std::move;
	using ::std::pair;
	using ::std::string_view;
	using ::std::vector;

	/// 不支持全局变量
	/// 不支持模板，以及非 JSON 包含的基本类型作为参数和返回值
	/// return type, name, args
	// array<string, 3> ParseFuncSignature(FuncDefTokenReader* reader);

	// WordEnumerator 还是有问题的，因为 start 到 end 可能跨 string_view 了，这就不行了
	/// body 是不包含函数的 { } 的
	// vector<string> ParseFuncBodyAfterSignature(FuncDefTokenReader* defReader);

	/// 使用前要保证函数编译正确，比如可以先编译一遍
	/// pair: { return type, name, args }, func body 
	vector<pair<array<string, 3>, vector<string>>> ParseFunc(FuncDefTokenReader* defReader);

	//如何改名字，让外面改和构造新函数
}

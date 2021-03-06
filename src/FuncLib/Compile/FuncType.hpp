#pragma once
#include <string>
#include <vector>
#include <string_view>

namespace FuncLib::Compile
{
	using ::std::string;
	using ::std::string_view;
	using ::std::vector;

	// 包含包名、函数名、返回值类型、参数类型
	class FuncType
	{
	public:
		string ReturnType;
		string FuncName;
		vector<string> ArgTypes;
		vector<string> Package;
		static FuncType FromKey(string_view key);
		FuncType() = default;
		FuncType(string returnType, string functionName, vector<string> argTypes);
		FuncType(string returnType, string functionName, vector<string> argTypes, vector<string> package);
		// 可以像 TiKV 那样对 Key 对 package name 做一些优化存储
		string ToKey() const;
		string ToString() const;
	};
}

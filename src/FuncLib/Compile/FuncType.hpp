#pragma once
#include <string>
#include <vector>

namespace FuncLib::Compile
{
	using ::std::move;
	using ::std::string;
	using ::std::vector;

	// 包含包名、函数名、返回值类型、参数类型（参数类型和名字，这个信息可以放到 summary 里面去）
	class FuncType
	{
	private:
		vector<string> _packageHierarchy;
		string _returnType;
		string _funcName;
		vector<string> _argTypes;
	public:
		FuncType(string returnType, string functionName, vector<string> argTypes);
		void PackageHierarchy(vector<string> packageHierarchy);
		void FuncName(string funcName);
		// 可以像 TiKV 那样对 Key 对 package name 做一些优化存储 TODO
		string ToKey() const;
		string ToString() const;
	};
}

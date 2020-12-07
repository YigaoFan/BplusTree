#include "FuncType.hpp"

namespace FuncLib::Compile
{
	using ::std::move;

	// 包含包名、函数名、返回值类型、参数类型（参数类型和名字，这个信息可以放到 summary 里面去）
	FuncType::FuncType(string returnType, string functionName, vector<string> argTypes)
		: FuncType(move(returnType), move(functionName), move(argTypes), {})
	{
	}

	FuncType::FuncType(string returnType, string functionName, vector<string> argTypes, vector<string> package)
		: ReturnType(move(returnType)), FuncName(move(functionName)), ArgTypes(move(argTypes)), PackageHierarchy(move(package))
	{
	}

	// 可以像 TiKV 那样对 Key 对 package name 做一些优化存储 TODO
	string FuncType::ToKey() const
	{
		string s;

		// 我希望排序的时候优先比较 pack，所以把包名放在了前面
		if (PackageHierarchy.empty())
		{
			char const *DefalutPackage = "Global";
			s.append(DefalutPackage);
		}
		else
		{
			for (auto& p : PackageHierarchy)
			{
				s.append(p + '.');
			}

			s.pop_back();
		}
		s.append(" ");

		s.append(ReturnType + ' ');
		s.append(FuncName + ' ');

		for (auto &a : ArgTypes)
		{
			s.append(a + ',');
		}
		s.pop_back();

		return s;
	}

	string FuncType::ToString() const
	{
		return ToKey();
	}
}

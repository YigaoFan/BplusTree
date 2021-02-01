#include "FuncType.hpp"
#include "../Basic/StringViewUtil.hpp"

namespace FuncLib::Compile
{
	using Basic::ParseOut;
	using Basic::TrimFirstChar;
	using ::std::move;

	vector<string> ParseZeroOrMore(string_view s, char divider)
	{
		vector<string> units;

		for (;not s.empty();)
		{
			auto [unit, remain] = ParseOut<false>(s, ".");
			s = remain;
			units.push_back(string(unit));
		}

		return units;
	}

	FuncType FuncType::FromKey(string_view key)
	{
		auto [packInfo, remain0] = ParseOut<true>(key, " ");
		vector<string> packs = ParseZeroOrMore(packInfo, '.');
		auto [returnType, remain1] = ParseOut<true>(remain0, " ");
		auto [funcName, argInfo] = ParseOut<true>(remain1, " ");
		vector<string> args = ParseZeroOrMore(argInfo, ',');

		return FuncType(string(returnType), string(funcName), move(args), move(packs));
	}

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

		// 我希望排序的时候优先比较 package，所以把包名放在了前面
		if (PackageHierarchy.empty())
		{
			char DefalutPackage[] = "Global";
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
		s.push_back(' ');

		s.append(ReturnType + ' ');
		s.append(FuncName + ' ');

		for (auto& a : ArgTypes)
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

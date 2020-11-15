#include <cassert>
#include <vector>
#include <tuple>
#include <utility>
#include <string_view>
#include "../Json/JsonConverter/WordEnumerator.hpp"

namespace FuncLib
{
	using Json::JsonConverter::WordEnumerator;
	using ::std::move;
	using ::std::pair;
	using ::std::string_view;
	using ::std::tuple;
	using ::std::vector;

	/// 不支持全局变量
	/// 不支持模板，以及非 JSON 包含的基本类型作为参数和返回值
	/// return type, name, args, current enumerator
	tuple<string_view, string_view, vector<string_view>, WordEnumerator> ParseFuncType(WordEnumerator defEnumerator)
	{
		defEnumerator.ChangeSeparator(' ');
		assert(defEnumerator.MoveNext());
		auto returnType = defEnumerator.Current();

		defEnumerator.ChangeSeparator('(');
		assert(defEnumerator.MoveNext());
		auto name = defEnumerator.Current();

		defEnumerator.ChangeSeparator(')');
		vector<string_view> args;

		if (defEnumerator.MoveNext())// 这里支持 () 这样吗？或者 () 的 MoveNext 返回值是？
		{
			auto argStr = defEnumerator.Current();
			WordEnumerator argsEnumerator{ {argStr}, ','};
			while (argsEnumerator.MoveNext()) // 这里要过滤可能出现的空格吗？
			{
				args.push_back(argsEnumerator.Current());
			}
		}

		return { returnType, name, move(args), move(defEnumerator) };
	}

	int Count(char c, string_view rangeStr)
	{
		int n = 0;
		for (auto x : rangeStr)
		{
			if (c == x)
			{
				++n;
			}
		}

		return n;
	}

	pair<vector<WordEnumerator>, vector<WordEnumerator>> GenerateFuncDeclareEnumerator(vector<string_view> strs)
	{
		auto e = WordEnumerator(strs, '{');
		vector<WordEnumerator> funcDeclareEnumerators;
		vector<WordEnumerator> funcBodyEnumerators;
		int unpairedCount = 0;

		while (e.MoveNext())
		{
			auto s = e.Current();
			auto c = Count('}', s);
			unpairedCount -= c;

			auto subE = WordEnumerator({ s }, ' ');
			if (unpairedCount == 0)
			{
				funcDeclareEnumerators.push_back(subE);
			}
			else
			{
				funcBodyEnumerators.push_back(subE);
			}
			

			unpairedCount += 1; // add for {
		}

		return { funcDeclareEnumerators, funcBodyEnumerators };
	}

	//如何改名字
}

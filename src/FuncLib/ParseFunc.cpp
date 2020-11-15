#include <cassert>
#include <vector>
#include <tuple>
#include <utility>
#include <string_view>
#include "../Basic/Exception.hpp"
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

	// TODO test
	// 要不要搞一种 divide enumerator 不去掉分隔符的那种
	WordEnumerator CollectFuncBody(WordEnumerator* defEnumerator)
	{
		// has a constructor for WordEnumerator({ s })
		auto start = defEnumerator->Current().end(); // end is {

		int unpairedCount = 1;
		defEnumerator->ChangeSeparator('}');
		while (defEnumerator->MoveNext())
		{
			auto s = defEnumerator->Current();
			auto c = Count('{', s);
			unpairedCount += c;
			--unpairedCount;

			if (unpairedCount == 0)
			{
				auto end = s.end() + 1; // end is }, need include, so + 1
				defEnumerator->ChangeSeparator('{');
				return WordEnumerator( { string_view(start, end - start) }, ' ');
			}
		}

		throw Basic::InvalidOperationException("{} in func body is unpaired");
	}

	vector<pair<WordEnumerator, WordEnumerator>> GenerateFuncDeclareEnumerator(vector<string_view> strs)
	{
		auto e = WordEnumerator(strs, '{');
		vector<pair<WordEnumerator, WordEnumerator>> funcs;

		while (e.MoveNext())
		{
			auto signature = WordEnumerator({ e.Current() }, ' ');
			auto body = CollectFuncBody(&e);
			funcs.push_back({ move(signature), move(body) });
		}

		return funcs;
	}

	//如何改名字，让外面改和构造新函数
}

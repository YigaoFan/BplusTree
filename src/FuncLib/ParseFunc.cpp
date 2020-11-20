#include <cassert>
#include <vector>
#include <tuple>
#include <utility>
#include <string_view>
#include <string>
#include "../Basic/Exception.hpp"
#include "FuncDefTokenReader.hpp"
#include "../Json/JsonConverter/WordEnumerator.hpp"// TODO remove

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
	tuple<string, string, string> ParseFuncType(FuncDefTokenReader* reader)
	{
		auto g = reader->GetTokenGenerator();
		reader->Delimiter(' ');
		g.Resume();// need assert?
		auto returnType = g.Current();

		reader->Delimiter('(');
		g.Resume();
		auto name = g.Current();

		reader->Delimiter(')');
		g.Resume();
		auto args = g.Current();

		return { move(returnType), move(name), move(args) };
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

	// WordEnumerator 还是有问题的，因为 start 到 end 可能跨 string_view 了，这就不行了
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

	vector<pair<WordEnumerator, WordEnumerator>> GenerateFuncEnumerator(vector<string_view> strs)
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

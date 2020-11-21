#include <string_view>
#include "../Basic/Exception.hpp"
#include "ParseFunc.hpp"

namespace FuncLib
{
	using ::std::move;
	using ::std::string_view;

	/// 不支持全局变量
	/// 不支持模板，以及非 JSON 包含的基本类型作为参数和返回值
	/// return type, name, args
	array<string, 3> ParseFuncSignature(FuncDefTokenReader* reader)
	{
		array<char, 3> delimiters
		{
			' ',
			'(',
			')',
		};
		array<string, 3> infos;
		auto g = reader->GetTokenGenerator();

		for (auto i = 0; auto d : delimiters)
		{
			reader->Delimiter(d);
			g.MoveNext(); // 不 assert 了，因为编译检查过了
			infos[i] = move(g.Current());
			++i;
		}

		return infos;
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
	/// body 是不包含函数的 { } 的
	vector<string> ParseFuncBodyAfterSignature(FuncDefTokenReader* defReader)
	{
		vector<string> funcBody;
		auto g = defReader->GetTokenGenerator();
		int unpairedCount = 1;
		defReader->Delimiter('}');

		while (g.MoveNext())
		{
			auto part = g.Current();
			unpairedCount += Count('{', part);
			--unpairedCount;
			funcBody.push_back(move(part));

			if (unpairedCount == 0)
			{
				return move(funcBody);
			}
		}

		throw Basic::InvalidOperationException("{} in func body is unpaired");
	}

	/// 使用前要保证函数编译正确，比如可以先编译一遍
	/// pair: { return type, name, args }, func body 
	vector<pair<array<string, 3>, vector<string>>> ParseFunc(FuncDefTokenReader* defReader)
	{
		vector<pair<array<string, 3>, vector<string>>> funcs;

		while (not defReader->AtEnd())
		{
			auto sign = ParseFuncSignature(defReader);
			auto body = ParseFuncBodyAfterSignature(defReader);

			funcs.push_back({ move(sign), move(body) });
		}

		return funcs;
	}
}

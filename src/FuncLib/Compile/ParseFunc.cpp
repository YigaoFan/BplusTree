#include <string_view>
#include <optional>
#include <cctype>
#include <array>
#include <utility>
#include <functional>
#include "../../Basic/Exception.hpp"
#include "ParseFunc.hpp"
#include "../../Json/JsonConverter/WordEnumerator.hpp"
#include "../Basic/StringViewUtil.hpp" // 原来不需要一级一级写上去

namespace FuncLib::Compile
{
	using Basic::ParseOut;
	using Basic::TrimEnd;
	using Basic::TrimStart;
	using Json::JsonConverter::WordEnumerator;
	using ::std::array;
	using ::std::function;
	using ::std::move;
	using ::std::optional;
	using ::std::pair;
	using ::std::string_view;

	/// 不支持全局变量
	/// 不支持模板，以及非 JSON 包含的基本类型作为参数和返回值
	/// return type, name, args
	optional<array<string, 3>> ParseFuncSignature(FuncDefTokenReader* reader)
	{
		// 这里假设源代码打开上来遇到的不是空白就是函数体
		array<pair<bool, function<bool(char)>>, 3> parseConfigs
		{
			// pair: 允许为空字符串, delimiter predicate
			pair<bool, function<bool(char)>>{ false, [](char c) { return std::isspace(c); } },
			{ false, [](char c) { return c == '('; } },
			{ true, [](char c) { return c == ')'; } },
		};
		
		array<string, 3> infos;
		auto g = reader->GetTokenGenerator();

		for (auto i = 0; auto c : parseConfigs)
		{
			reader->DelimiterPredicate(c.second);
			while (g.MoveNext())
			{
				auto t = g.Current();
				if (c.first or not t.empty())
				{
					infos[i++] = move(g.Current());
					goto Continue;
				}
			}
			
			return {};
		Continue:
			continue;
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
		int unpairedCount = 0;
		defReader->DelimiterPredicate([](char c) { return c == '}'; });

		while (g.MoveNext())
		{
			auto part = g.Current();
			unpairedCount += Count('{', part);
			--unpairedCount;

			if (unpairedCount == 0)
			{
				funcBody.push_back(move(part));
				return funcBody;
			}
			else
			{
				funcBody.push_back(move(part) + '}');
			}
		}

		return funcBody;
	}

	/// pair: type and parameter name
	pair<vector<string>, vector<string>> ParseOutParas(string_view parasStr)
	{
		function<pair<vector<string>, vector<string>>(string_view)> GetParaTypeNamesFrom = [&](string_view s) -> pair<vector<string>, vector<string>>
		{
			if (s.empty()) { return {}; }

			auto [typeAndName, remain] = ParseOut<true>(s, ",");
			auto [type, name] = ParseOut<true>(typeAndName, " ");
			if (remain.empty())
			{
				return { { string(type) }, { string(name) } };
			}

			auto remainTypeNames = GetParaTypeNamesFrom(remain);
			remainTypeNames.first.insert(remainTypeNames.first.begin(), string(type));
			remainTypeNames.second.insert(remainTypeNames.second.begin(), string(name));
			return remainTypeNames;
		};
		
		return GetParaTypeNamesFrom(parasStr);
	}

	// TODO 准确定位到函数类型签名，忽略 using 和 include 语句
	/// tuple: FuncType, para name, func body 
	vector<tuple<FuncType, vector<string>, vector<string>>> ParseFunc(FuncDefTokenReader& defReader)
	{
		defReader.ResetReadPos();
		vector<tuple<FuncType, vector<string>, vector<string>>> funcs;

		while (not defReader.AtEnd())
		{
			if (auto sign = ParseFuncSignature(&defReader); sign.has_value())
			{
				auto body = ParseFuncBodyAfterSignature(&defReader);
				auto& s = sign.value();
				auto [paraType, paraName] = ParseOutParas(s[2]);
				funcs.push_back({ 
					FuncType(move(s[0]), move(s[1]), move(paraType)),
					move(paraName),
					move(body) });
			}
		}

		return funcs;
	}
}

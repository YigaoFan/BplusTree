#include <string_view>
#include <optional>
#include <cctype>
#include <array>
#include <utility>
#include <functional>
#include "../../Basic/Exception.hpp"
#include "ParseFunc.hpp"
#include "../../Json/JsonConverter/WordEnumerator.hpp"

namespace FuncLib::Compile
{
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
				return move(funcBody);
			}
			else
			{
				funcBody.push_back(move(part) + '}');
			}
		}

		return funcBody;
		// throw Basic::InvalidOperationException("{} in func body is unpaired");
	}

	/// pair: type and parameter name
	pair<vector<string>, vector<string>> ParseOutParas(string const &argsStr)
	{
		auto divideArg = [](string_view s, char delimiter)
		{
			auto i = s.find_first_of(delimiter);
			return array<string_view, 2>
			{
				s.substr(0, i), 
				s.substr(i)
			};
		};

		WordEnumerator e{ vector<string_view>{ argsStr }, ','};
		pair<vector<string>, vector<string>> paras;
		while (e.MoveNext())
		{
			if (not e.Current().empty())
			{
				auto [type, name] = divideArg(e.Current(), ' ');
				paras.first.push_back(string(type));
				paras.second.push_back(string(name));
			}
		}

		return paras;
	}

	/// tuple: FuncType, para name, func body 
	vector<tuple<FuncType, vector<string>, vector<string>>> ParseFunc(FuncDefTokenReader* defReader)
	{
		vector<tuple<FuncType, vector<string>, vector<string>>> funcs;

		while (not defReader->AtEnd())
		{
			if (auto sign = ParseFuncSignature(defReader); sign.has_value())
			{
				auto body = ParseFuncBodyAfterSignature(defReader);
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

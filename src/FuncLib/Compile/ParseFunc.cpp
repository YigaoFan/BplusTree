#include <string_view>
#include <optional>
#include <cctype>
#include <array>
#include <utility>
#include <functional>
#include "../Basic/Exception.hpp"
#include "ParseFunc.hpp"
#include "../Json/JsonConverter/WordEnumerator.hpp"
#include "../Basic/StringViewUtil.hpp" // 原来不需要一级一级写上去

// temp
#include <string>
#include <utility>
#include <queue>

namespace FuncLib::Compile
{
	using Basic::ParseOut;
	using Basic::TrimEnd;
	using Basic::TrimStart;
	using Basic::TrimFirstChar;
	using Json::JsonConverter::WordEnumerator;
	using ::std::array;
	using ::std::function;
	using ::std::move;
	using ::std::optional;
	using ::std::pair;
	using ::std::string_view;

	using ::std::string;
	using ::std::make_index_sequence;
	using ::std::index_sequence;
	using ::std::queue;

	// Parser Combinator inspired from: https://zhuanlan.zhihu.com/p/249005405
	using ParserInput = string_view;
	template <typename T>
	using ParserResult = optional<pair<T, ParserInput>>;
	template <typename T>
	using Parser = auto(*)(ParserInput) -> ParserResult<T>;

	template <typename T>
	struct GetDestTypeOfParser
	{
		using Result = typename std::invoke_result_t<T, ParserInput>::value_type::first_type;
	};

	template <typename T>
	struct GetDestTypeOfParser<auto (*)(ParserInput) -> ParserResult<T>>
	{
		using Result = T;
	};
	
	template <typename T>
	using ResultOfGetDestTypeOfParser = typename GetDestTypeOfParser<T>::Result;

	constexpr auto Combine(auto parser1, auto parser2, auto resultProcessor)
	{
		return [parser1=move(parser1), parser2=move(parser2), resultProcessor=move(resultProcessor)](ParserInput s) -> ParserResult<std::invoke_result_t<decltype(resultProcessor), ResultOfGetDestTypeOfParser<decltype(parser1)>, ResultOfGetDestTypeOfParser<decltype(parser2)>>>
		{
			if (auto r1 = parser1(s); r1.has_value())
			{
				if (auto r2 = parser2(r1->second); r2.has_value())
				{
					return pair(resultProcessor(move(r1->first), move(r2->first)), r2->second);
				}
			}

			return std::nullopt;
		};
	}

	constexpr auto MakeStopWhileEncounterParser(auto pred)
	{
		return [pred=move(pred)](ParserInput s) -> ParserResult<string>
		{
			for (auto i = 0; i < s.size(); ++i)
			{
				if (pred(s[i]))
				{
					return pair(string(s.substr(0, i)), s.substr(i));
				}
			}

			return pair(string(s), string_view());
		};
	}

	/// Result not include c
	/// 如果不含 c，则返回全字符串作为 pair 中的 first
	constexpr auto MakeStopWhileEncounterParser(char c)
	{
		return [=](ParserInput s) -> ParserResult<string>
		{
			for (auto i = 0; i < s.size(); ++i)
			{
				if (c == s[i])
				{
					// 返回值都变成 string_view 吧 TODO
					return pair(string(s.substr(0, i)), s.substr(i));
				}
			}

			return pair(string(s), ParserInput());
		};
	}

	// 为什么这个就不用放到 std namespace 里，上次在 ByteConverter 那里就用
	// 不过那次是加两个 array，然后使用的地方是在 ByteConverter 特化的类里面，可能依赖查找的地方就不一样了
	template <typename T, auto N>
	array<T, N + 1> operator+ (array<T, N> ts, T t)
	{
		auto add = [&]<size_t... Idxs>(index_sequence<Idxs...>)
		{
			return array<T, N + 1>
			{
				move(ts[Idxs])...,
				move(t),
			};
		};

		auto idxs = make_index_sequence<N>();
		return add(idxs);
	}

	template <typename T>
	array<T, 2> operator+ (T t1, T t2)
	{
		return array<T, 2>
		{
			move(t1),
			move(t2),
		};
	}

	/// 两个 parser，然后也有相应的结果处理就使用这种
	template <typename Parser2, typename ResultProcessor>
	constexpr auto operator> (auto parser, pair<Parser2, ResultProcessor> parserAndResultProcessor)
	{
		return Combine(move(parser), move(parserAndResultProcessor.first), move(parserAndResultProcessor.second));
	}

	constexpr auto operator> (auto parser, auto remainStrProcessor)
	{
		return [parser=move(parser), remainStrProcessor=move(remainStrProcessor)](string_view s) -> std::invoke_result_t<decltype(parser), decltype(s)>
		{
			auto r = parser(s);
			if (r.has_value())
			{
				return pair(move(r->first), remainStrProcessor(r->second));
			}

			return r;
		};
	}

	constexpr auto operator< (auto parser, auto resultProcessor)
	{
		return [parser=move(parser), resultProcessor=move(resultProcessor)](string_view s) -> ParserResult<std::invoke_result_t<decltype(resultProcessor), ResultOfGetDestTypeOfParser<decltype(parser)>>>
		{
			auto r = parser(s);
			if (r.has_value())
			{
				return pair(resultProcessor(move(r->first)), r->second);
			}

			return r;
		};
	}

	// 也可以想办法逆着解析
	/// rangeParser is ParserResult<string_view>
	constexpr auto LastTwo(auto rangeParser, auto unitParser)
	{
		return [rangeParser=move(rangeParser), unitParser=move(unitParser)](string_view s) -> ParserResult<array<ResultOfGetDestTypeOfParser<decltype(unitParser)>, 2>>
		{
			if (auto r1 = rangeParser(s); r1.has_value())
			{
				using T = ResultOfGetDestTypeOfParser<decltype(unitParser)>;
				queue<T> results;
				auto range = r1->first;

				while (not range.empty())
				{
					if (auto r2 = unitParser(range); r2.has_value())
					{
						if (results.size() == 2)
						{
							results.pop();
						}
						results.push(move(r2->first));
						range = TrimFirstChar(move(r2->second));
						continue;
					}
					break;
				}

				if (results.size() == 2)
				{
					return pair(array<T, 2>{ move(results.front()), move(results.back()) }, r1->second); // 改返回值类型，检查这个程序
				}
			}

			return std::nullopt;
		};
	}

	constexpr auto MakeFuncSignParser()
	{
		auto trimStart = [](auto s)
		{
			auto subView = TrimStart(s);
			if constexpr (std::is_same_v<decltype(s), string>)
			{
				return string(subView);
			}
			else
			{
				return subView;
			}
		};

		// sign 是合要求的可以保证我们解析到的是正确位置的 sign
		auto checkStrAfterSign = [](auto sign, auto strAfterSign) -> optional<decltype(sign)>
		{
			if (not strAfterSign.empty())
			{
				return std::nullopt;
			}
			return sign;
		};

		auto trimEnd = [](auto s)
		{
			auto subView = TrimEnd(s);
			if constexpr (std::is_same_v<decltype(s), string>)
			{
				return string(subView);
			}
			else
			{
				return subView;
			}
		};

		auto trimFirstChar = [](auto s)
		{
			return s.substr(1);
		};
		auto isSpace = [](char c) { return std::isspace(c); };

		return LastTwo(MakeStopWhileEncounterParser('('), MakeStopWhileEncounterParser(isSpace))  // return type and func name
			> trimFirstChar
			> pair(MakeStopWhileEncounterParser(')') < trimStart < trimEnd, &operator+<string, 2>)
			> trimFirstChar
			> pair(MakeStopWhileEncounterParser('{') < trimStart < trimEnd, checkStrAfterSign);
	}

	/// 不支持全局变量
	/// 不支持模板，以及非 JSON 包含的基本类型作为参数和返回值
	/// return type, name, args
	// optional<array<string, 3>> ParseFuncSignature(FuncDefTokenReader* reader)
	// {
	// 	// 这里假设源代码打开上来遇到的不是空白就是函数体
	// 	array<pair<bool, function<bool(char)>>, 3> parseConfigs
	// 	{
	// 		// pair: 允许为空字符串, delimiter predicate
	// 		pair<bool, function<bool(char)>>{ false, [](char c) { return std::isspace(c); } },
	// 		{ false, [](char c) { return c == '('; } },
	// 		{ true, [](char c) { return c == ')'; } },
	// 	};
		
	// 	array<string, 3> infos;
	// 	auto g = reader->GetTokenGenerator();

	// 	for (auto i = 0; auto c : parseConfigs)
	// 	{
	// 		reader->DelimiterPredicate(c.second);
	// 		while (g.MoveNext())
	// 		{
	// 			auto t = g.Current();
	// 			if (c.first or not t.empty())
	// 			{
	// 				infos[i++] = move(g.Current());
	// 				goto Continue;
	// 			}
	// 		}
			
	// 		return {};
	// 	Continue:
	// 		continue;
	// 	}

	// 	return infos;
	// }

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

	/// body 是包含函数的 { } 的
	pair<bool, size_t> CheckBracesBalance(string_view s, size_t& i)
	{
		auto balance = true;
		for (; i < s.size();)
		{
			// 这个检测有缺陷的地方在于有些需要忽略的地方没有忽略——注释和字符串里也可能有 { 符号 
			if (s[i] == '{')
			{
				if (balance)
				{
					++i;
					balance = false;
				}

				if (auto r = CheckBracesBalance(s, i); not r.first)
				{
					return r;
				}
			}
			else if (s[i] == '}')
			{
				if (not balance)
				{
					++i;
				}

				return pair(true, i);
			}
			else
			{
				++i;
			}
		}

		return pair(balance, s.size());
	}

	ParserResult<bool> CheckFuncBody(string_view s)
	{
		size_t i = 0;
		if (auto r = CheckBracesBalance(s, i); r.first)
		{
			return pair(true, s.substr(r.second)); // 即使这个 r.second 是 end 位置也安全的返回了空 string_view，看下文档是否是明确规定了
		}

		return std::nullopt;
	}
	// 这都可以，可以当扩展方法用了，但是怎么查找到这个函数上来的？
	auto operator| (auto s, auto processor)
	{
		return processor(s);
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

		return GetParaTypeNamesFrom(parasStr | TrimEnd | TrimStart);
	}

	// TODO 准确定位到函数类型签名，忽略 using 和 include 语句
	/// tuple: FuncType, para name, func body 
	vector<tuple<FuncType, vector<string>, vector<string>>> ParseFunc(string_view code)
	{
		vector<tuple<FuncType, vector<string>, vector<string>>> funcs;
		auto signParser = MakeFuncSignParser();
		auto first = true;
		auto tokenParser = MakeStopWhileEncounterParser([&first](char c) { if (first) { first = false; return true; } return static_cast<bool>(std::isspace(c)); }) > TrimStart;

		for (auto tokenResult = tokenParser(code); tokenResult.has_value() and not tokenResult.value().second.empty(); )
		{
			auto remainCode = tokenResult.value().second;

			// sign parser 里面要加下错误处理
			if (auto r = signParser(remainCode); r.has_value())
			{
				auto inner = move(r.value());
				if (inner.first.has_value())
				{
					auto [returnType, funcName, paraStr] = move(inner.first.value());
					auto [paraTypes, paraNames] = ParseOutParas(paraStr);

					vector<string> body; // TODO
					funcs.push_back({FuncType(move(returnType), move(funcName), move(paraTypes)),
									 move(paraNames),
									 move(body)});

					auto r = CheckFuncBody(inner.second);
					// assert(r.has_value()); // TODO handle
					remainCode = r.value().second;
				}
			}

			tokenResult = tokenParser(remainCode);
		}

		return funcs;
	}
}

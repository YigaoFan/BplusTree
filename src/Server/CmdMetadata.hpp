#pragma once
#include <tuple>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <stdexcept>
#include <functional>
#include <string_view>
#include "Request.hpp"

namespace Server
{
	using FuncLib::Compile::FuncType;
	using ::std::find_if_not;
	using ::std::forward;
	using ::std::function;
	using ::std::invalid_argument;
	using ::std::isspace;
	using ::std::move;
	using ::std::pair;
	using ::std::string;
	using ::std::string_view;
	using ::std::tuple;
	using ::std::vector;

	//如何设置 block 的时限呢？
	// socket->set_option(asio::detail::socket_option::integer<SOL_SOCKET, SO_RCVTIMEO>{200});

#define nameof(VAR) #VAR
	/// Get admin command names
	vector<string> GetCmdsName()
	{
		return
		{
			nameof(AddFunc),
			nameof(RemoveFunc),
			nameof(SearchFunc),
			nameof(ModifyFunc),
		};
	}

	template <typename Content>
	string Serial(Content content)
	{
		return Json::JsonConverter::Serialize(content).ToString();
	}

	template <typename To, typename... Elements>
	To CombineTo(Elements&&... elements)
	{
		return
		{
			forward<Elements>(elements)...,
		};
	}

	string_view TrimStart(string_view s)
	{
		auto isSpace = [](char c) { return isspace(c); };
		return s.substr(find_if_not(s.begin(), s.end(), isSpace) - s.begin());
	}

	string_view TrimEnd(string_view s)
	{
		auto isSpace = [](char c) { return isspace(c); };
		return s.substr(0, find_if_not(s.rbegin(), s.rend(), isSpace).base() - s.begin());
	}

	string_view RemoveFirst(string_view keyword, string_view s)
	{
		return s.substr(s.find_first_of(keyword) + keyword.size());
	}

	/// Remove first cmd name in inputCmd with trim start and trim end
	string_view Preprocess(string_view cmdName, string inputCmd)
	{
		return TrimStart(RemoveFirst(cmdName, TrimEnd(TrimStart(inputCmd))));
	}

	/// 调用者要保证 divider 确在，否则会抛异常
	template <bool IsTrimRemainStart>
	pair<string_view, string_view> ParseOut(string_view s, string_view divider)
	{
		auto dividerPos = s.find_first_of(divider);
		auto content = s.substr(0, dividerPos);
		string_view remain;

		if (dividerPos != string_view::npos)
		{
			remain = s.substr(dividerPos + divider.size());
			if constexpr (IsTrimRemainStart)
			{
				remain = TrimStart(remain);
			}

		}
		
		return { content, remain };
	}

	/// Package divided with '.'
	vector<string> GetPackageFrom(string_view packageInfo)
	{
		packageInfo = TrimStart(TrimEnd(packageInfo));

		auto [package, remain] = ParseOut<true>(packageInfo, ".");
		if (package.empty())
		{
			return {};
		}

		if (remain.empty())
		{
			return { string(package), };
		}

		auto remainPackage = GetPackageFrom(remain);
		remainPackage.insert(remainPackage.begin(), string(package));
		return remainPackage;
	}

	// A.B::void A(argT1, argT2)
	FuncType GetFuncTypeFrom(string_view funcInfo)
	{
		auto [packageInfo, remain0] = ParseOut<true>(funcInfo, "::");
		auto package = GetPackageFrom(packageInfo);

		auto [returnType, remain1] = ParseOut<true>(remain0, " ");

		auto [funcName, remain2] = ParseOut<true>(remain1, "(");
		funcName = TrimEnd(funcName);

		auto [argTypeInfo, remain3] = ParseOut<true>(remain2, ")");
		function<vector<string>(string_view)> GetArgTypesFrom = [&](string_view s) -> vector<string>
		{
			if (s.empty()) { return {}; }

			auto [type, remain] = ParseOut<true>(s, ",");
			if (remain.empty())
			{
				return vector<string>{ string(type), };
			}

			auto remainTypes = GetArgTypesFrom(remain);
			remainTypes.insert(remainTypes.begin(), string(type));
			return remainTypes;
		};
		vector<string> argTypes = GetArgTypesFrom(argTypeInfo);

		return FuncType(move(string(returnType)), move(string(funcName)), move(argTypes), move(package));
	}

	class AddFuncCmd
	{
	private:
		static string GetFuncsDefFrom(string_view filename)
		{
			using ::std::ifstream;
			using ::std::istreambuf_iterator;
			ifstream fs(filename, ifstream::in);
			fs.seekg(0, ifstream::end);
			auto n = fs.tellg();
			string def;
			def.resize(n);
			fs.seekg(0); // restore read position
			fs.read(def.data(), n);

			return def;
		}

		string GetSummaryFrom(string_view summaryPartOfCmd)
		{
			return string(summaryPartOfCmd);
		}

		tuple<string_view, string_view, string_view> DivideInfo(string_view cmd)
		{
			auto [packagePart, remain] = ParseOut<true>(cmd, " ");
			auto [filename, summaryPart] = ParseOut<true>(remain, " ");

			return { packagePart, filename, summaryPart };
		}

	public:
		/// Process command to the content be sent to server
		string Process(string cmd)
		{
			auto [packagePart, filename, summaryPart] = DivideInfo(Preprocess(nameof(AddFunc), cmd));
			vector<string> package = GetPackageFrom(packagePart);
			string funcsDef = GetFuncsDefFrom(filename);
			string summary = GetSummaryFrom(summaryPart);

			return Serial(CombineTo<AddFuncRequest::Content>(
				move(package),
				move(funcsDef),
				move(summary)));
		}
	};

	// Modify 和 Remove 应该在函数唯一的情况下，允许用户简写参数，但要返回来让用户确认 TODO
	struct RemoveFuncCmd
	{
		string Process(string cmd)
		{
			auto funcInfo = Preprocess(nameof(RemoveFunc), cmd);
			return Serial(CombineTo<RemoveFuncRequest::Content>(GetFuncTypeFrom(funcInfo)));
		}
	};

	struct SearchFuncCmd
	{
		string Process(string cmd)
		{
			auto keyword = Preprocess(nameof(SearchFunc), cmd);
			return Serial(CombineTo<SearchFuncRequest::Content>(string(keyword)));
		}
	};

	// Sample: ModifyFuncPackage A.B::void A(argT1, argT2) C.D
	class ModifyFuncPackageCmd
	{
	private:
		tuple<string_view, string_view> DivideInfo(string_view cmd)
		{
			auto [newPackageInfo, funcInfo] = ParseOut<true>(cmd, "::");
			return { funcInfo, newPackageInfo };
		}

	public:
		string Process(string cmd)
		{
			auto [funcInfo, newPackageInfo] = DivideInfo(Preprocess(nameof(ModifyFuncPackage), cmd));

			auto func = GetFuncTypeFrom(funcInfo);
			auto newPackage = GetPackageFrom(newPackageInfo);

			return Serial(CombineTo<ModifyFuncPackageRequest::Content>(move(func), move(newPackage)));
		}
	};

	constexpr unsigned int StrToInt(char const* str, int h = 0)
	{
		return !str[h] ? 5381 : (StrToInt(str, h + 1) * 33) ^ str[h];
	}

	string GenerateSendBytes(string cmd)
	{
		auto [cmdName, unused] = ParseOut<false>(cmd, " ");
		switch (StrToInt(cmd.c_str()))
		{
#define CASE_OF(NAME) case StrToInt(nameof(NAME)): return NAME##Cmd().Process(cmd)

		CASE_OF(AddFunc);
		CASE_OF(RemoveFunc);
		CASE_OF(SearchFunc);
		CASE_OF(ModifyFuncPackage);
		default: throw invalid_argument(string("No handler of ") + cmd);

#undef CASE_OF
		}
	}
#undef nameof
}

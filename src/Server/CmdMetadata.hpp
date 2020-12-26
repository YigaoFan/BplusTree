#pragma once
#include <array>
#include <tuple>
#include <vector>
#include <string>
#include <fstream>
#include <string_view>
#include "Request.hpp"

namespace Server
{
	using FuncLib::Compile::FuncType;
	using ::std::array;
	using ::std::find_if_not;
	using ::std::forward;
	using ::std::isspace;
	using ::std::move;
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
		return s.substr(s.find_first_of(keyword));
	}

	/// Remove first cmd name in inputCmd with trim start and trim end
	string_view Preprocess(string_view cmdName, string inputCmd)
	{
		return TrimStart(RemoveFirst(cmdName, TrimEnd(TrimStart(inputCmd))));
	}

	/// Package divided with '.'
	vector<string> GetPackageFrom(string_view packageInfo)
	{
		auto pointPos = packageInfo.find_first_of('.');
		auto package = packageInfo.substr(0, pointPos);
		auto remainPackage = GetPackageFrom(packageInfo.substr(pointPos + 1));
		remainPackage.insert(remainPackage.begin(), string(package));
		return remainPackage;
	}

	// A.B::void A(argT1, argT2)
	// TODO 简化下面的代码
	FuncType GetFuncTypeFrom(string_view funcInfo)
	{
		auto dividerPos = funcInfo.find_first_of("::");
		auto package = GetPackageFrom(funcInfo.substr(0, dividerPos));
		auto remain = TrimStart(funcInfo.substr(dividerPos + 2)); // 可能 + 2 位置不对

		dividerPos = remain.find_first_of(' ');
		auto returnType = string(remain.substr(0, dividerPos));
		remain = TrimStart(remain.substr(dividerPos + 1));

		auto leftParenthesePos = remain.find_first_of('(');
		auto rightParenthesePos = remain.find_first_of(')');
		auto funcName = string(TrimEnd(remain.substr(0, leftParenthesePos)));
		auto argTypeInfo = TrimEnd(TrimStart(remain.substr(leftParenthesePos + 1, rightParenthesePos)));

		vector<string> argTypes;
		while (true)
		{
			dividerPos = argTypeInfo.find_first_of(',');
			if (dividerPos == string_view::npos)
			{
				break;
			}
			else
			{
				auto t = string(argTypeInfo.substr(0, dividerPos));
				argTypes.push_back(move(t));

				argTypeInfo = TrimStart(argTypeInfo.substr(dividerPos + 1));
			}
		}

		return FuncType(move(returnType), move(funcName), move(argTypes), move(package));
	}

	struct AddFuncCmd
	{
		string GetFuncsDefFrom(string_view filename)
		{
			using ::std::ifstream;
			using ::std::istreambuf_iterator;
			ifstream fs(filename, ifstream::in);

			return string(istreambuf_iterator<char>(fs), istreambuf_iterator<char>());
		}

		string GetSummaryFrom(string_view summaryPartOfCmd)
		{
			return string(summaryPartOfCmd);
		}

		tuple<string_view, string_view, string_view> DivideInfo(string_view cmd)
		{
			auto dividerPos = cmd.find_first_of(' ');
			auto packagePart = cmd.substr(0, dividerPos);
			cmd = TrimStart(cmd.substr(dividerPos + 1));

			dividerPos = cmd.find_first_of(' ');
			auto filename = cmd.substr(0, dividerPos);
			cmd = TrimStart(cmd.substr(dividerPos + 1));

			auto summaryPart = cmd.substr(0, cmd.find_first_of(' '));

			return { packagePart, filename, summaryPart };
		}


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
			auto funcInfo = TrimStart(Preprocess(nameof(RemoveFunc), cmd));
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
	struct ModifyFuncPackageCmd
	{
		tuple<string_view, string_view> DivideInfo(string_view cmd)
		{
			auto dividerPos = cmd.find_last_of(' ');
			auto funcInfo = TrimEnd(cmd.substr(0, dividerPos));
			auto newPackageInfo = cmd.substr(dividerPos + 1);

			return { funcInfo, newPackageInfo };
		}

		string Process(string cmd)
		{
			auto [funcInfo, newPackageInfo] = DivideInfo(Preprocess(nameof(ModifyFuncPackage), cmd));

			auto func = GetFuncTypeFrom(funcInfo);
			auto newPackage = GetPackageFrom(newPackageInfo);

			return Serial(CombineTo<ModifyFuncPackageRequest::Content>(move(func), move(newPackage)));
		}
	};
#undef nameof
}

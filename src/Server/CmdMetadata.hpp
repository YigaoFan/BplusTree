#pragma once
#include <tuple>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <stdexcept>
#include <functional>
#include <string_view>
#include "../Basic/StringViewUtil.hpp"
#include "Request.hpp"

namespace Server
{
	using Basic::ParseOut;
	using Basic::TrimEnd;
	using Basic::TrimStart;
	using FuncLib::Compile::FuncType;
	using ::std::forward;
	using ::std::function;
	using ::std::invalid_argument;
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
			nameof(ContainsFunc),
			nameof(AddClientAccount),
			nameof(RemoveClientAccount),
			nameof(AddAdminAccount),
			nameof(RemoveAdminAccount),
		};
	}

	template <typename Content>
	JsonObject Serial(Content const& content)
	{
		return Json::JsonConverter::Serialize(content);
	}

	template <typename To, typename... Elements>
	To CombineTo(Elements&&... elements)
	{
		return
		{
			forward<Elements>(elements)...,
		};
	}

	/// keyword 要确定存在，否则会报错
	string_view RemoveFirst(string_view keyword, string_view s)
	{
		if (auto p = s.find(keyword); p != string_view::npos)
		{
			return s.substr(p + keyword.size());
		}

		throw invalid_argument(string(s) + " no include " + string(keyword));
	}

	/// Remove first cmd name in inputCmd with trim start and trim end
	string_view Preprocess(string_view cmdName, string_view inputCmd)
	{
		return TrimStart(RemoveFirst(cmdName, TrimEnd(TrimStart(inputCmd))));
	}

	/// Package divided with '.'
	vector<string> GetPackageFrom(string_view packageInfo)
	{
		packageInfo = TrimStart(TrimEnd(packageInfo));
		if (packageInfo.empty())
		{
			return {};
		}
		
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
		function<vector<string>(string_view)> GetParaTypesFrom = [&GetParaTypesFrom](string_view s) -> vector<string>
		{
			if (s.empty()) { return {}; }
			
			auto [type, remain] = ParseOut<true>(s, ",");
			if (remain.empty())
			{
				return vector<string>{ string(type), };
			}

			auto remainTypes = GetParaTypesFrom(remain);
			remainTypes.insert(remainTypes.begin(), string(type));
			return remainTypes;
		};
		vector<string> argTypes = GetParaTypesFrom(argTypeInfo);

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
			string def(n, ' ');
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
		JsonObject Process(string_view cmd)
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
		JsonObject Process(string_view cmd)
		{
			auto funcInfo = Preprocess(nameof(RemoveFunc), cmd);
			auto type = GetFuncTypeFrom(funcInfo);
			auto j = Json::JsonConverter::Serialize(RemoveFuncRequest::Content{ type });
			return j;
		}
	};

	struct SearchFuncCmd
	{
		JsonObject Process(string_view cmd)
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
			auto p = cmd.find_last_of(' ');
			auto funcInfo = TrimEnd(cmd.substr(0, p));
			auto newPackageInfo = cmd.substr(p + 1);
			return { funcInfo, newPackageInfo };
		}

	public:
		JsonObject Process(string_view cmd)
		{
			auto [funcInfo, newPackageInfo] = DivideInfo(Preprocess(nameof(ModifyFuncPackage), cmd));

			auto func = GetFuncTypeFrom(funcInfo);
			auto newPackage = GetPackageFrom(newPackageInfo);

			return Serial(CombineTo<ModifyFuncPackageRequest::Content>(move(func), move(newPackage)));
		}
	};

	struct ContainsFuncCmd
	{
		JsonObject Process(string_view cmd)
		{
			auto funcInfo = Preprocess(nameof(ContainsFunc), cmd);
			return Serial(CombineTo<ContainsFuncRequest::Content>(GetFuncTypeFrom(funcInfo)));
		}
	};

	struct AddClientAccountCmd
	{
		JsonObject Process(string_view cmd)
		{
			auto accountInfo = Preprocess(nameof(AddClientAccount), cmd);
			auto [username, password] = ParseOut<true>(accountInfo, " ");
			return Serial(CombineTo<AddClientAccountRequest::Content>(string(username), string(password)));
		}
	};

	struct RemoveClientAccountCmd
	{
		JsonObject Process(string_view cmd)
		{
			auto username = Preprocess(nameof(RemoveClientAccount), cmd);
			return Serial(CombineTo<RemoveClientAccountRequest::Content>(string(username)));
		}
	};

	struct AddAdminAccountCmd
	{
		JsonObject Process(string_view cmd)
		{
			auto accountInfo = Preprocess(nameof(AddAdminAccount), cmd);
			auto [username, password] = ParseOut<true>(accountInfo, " ");
			return Serial(CombineTo<AddAdminAccountRequest::Content>(string(username), string(password)));
		}
	};

	struct RemoveAdminAccountCmd
	{
		JsonObject Process(string_view cmd)
		{
			auto username = Preprocess(nameof(RemoveAdminAccount), cmd);
			return Serial(CombineTo<RemoveClientAccountRequest::Content>(string(username)));
		}
	};

	constexpr unsigned int StrToInt(string_view str)
	{
		if (not str.empty())
		{
			return (StrToInt(str.substr(1)) * 33) ^ str[0];
		}
		return 5381;
	}

	string GenerateSendBytes(string const& cmd)
	{
		auto [cmdName, remain] = ParseOut<false>(TrimStart(cmd), " ");
		switch (StrToInt(cmdName))
		{
#define CASE_OF(NAME) case StrToInt(nameof(NAME)): return NAME##Cmd().Process(cmd).ToString()

		CASE_OF(AddFunc);
		CASE_OF(RemoveFunc);
		CASE_OF(SearchFunc);
		CASE_OF(ModifyFuncPackage);
		CASE_OF(ContainsFunc);
		CASE_OF(AddClientAccount);
		CASE_OF(AddAdminAccount);
		CASE_OF(RemoveClientAccount);
		CASE_OF(RemoveAdminAccount);
		default: throw invalid_argument(string("No handler of ") + cmd);

#undef CASE_OF
		}
	}
#undef nameof
}

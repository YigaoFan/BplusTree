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
#include "../Network/Request.hpp"
#include "../Json/Json.hpp"
#include "../Network/Util.hpp"
#include "PredefineHeader.hpp"

namespace Server
{
	using Basic::ParseOut;
	using Basic::TrimEnd;
	using Basic::TrimStart;
	using FuncLib::Compile::FuncType;
	using Json::JsonObject;
	using Network::AddAdminAccountRequest;
	using Network::AddClientAccountRequest;
	using Network::AddFuncRequest;
	using Network::ContainsFuncRequest;
	using Network::GetFuncsInfoRequest;
	using Network::HandleOperationResponse;
	using Network::InvokeFuncRequest;
	using Network::ModifyFuncPackageRequest;
	using Network::RemoveAdminAccountRequest;
	using Network::RemoveClientAccountRequest;
	using Network::RemoveFuncRequest;
	using Network::SearchFuncRequest;
	using ::std::declval;
	using ::std::forward;
	using ::std::function;
	using ::std::invalid_argument;
	using ::std::move;
	using ::std::ofstream;
	using ::std::pair;
	using ::std::string;
	using ::std::string_view;
	using ::std::tuple;
	using ::std::vector;

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
			nameof(GetFuncsInfo),
			nameof(AddClientAccount),
			nameof(RemoveClientAccount),
			nameof(AddAdminAccount),
			nameof(RemoveAdminAccount),
			nameof(Shutdown),
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
	// string_view Preprocess(string_view cmdName, string_view inputCmd)
	// {
	// 	return TrimStart(RemoveFirst(cmdName, TrimEnd(TrimStart(inputCmd))));
	// }

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

	constexpr char SuccessTip[] = "operate succssed";

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
			ifstream fs(filename);
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

		tuple<string_view, string_view, string_view> DivideInfo(string_view args)
		{
			auto [packagePart, remain] = ParseOut<true>(args, " ");
			auto [filename, summaryPart] = ParseOut<true>(remain, " ");

			return { packagePart, filename, summaryPart };
		}

	public:
		/// ProcessArg command to the content be sent to server
		/// 改成 JsonObject 返回值是方便测试，可以从中读信息，返回 string 读就比较难
		JsonObject ProcessArg(string_view args)
		{
			auto [packagePart, filename, summaryPart] = DivideInfo(args);
			vector<string> package = GetPackageFrom(packagePart);
			string funcsDef = GetFuncsDefFrom(filename);
			string summary = GetSummaryFrom(summaryPart);

			return Serial(CombineTo<AddFuncRequest::Content>(
				move(package),
				move(funcsDef),
				move(summary)));
		}

		vector<string> ProcessResponse(string_view response)
		{
			HandleOperationResponse<void>(response);
			return { SuccessTip };
		}
	};

	// Modify 和 Remove 应该在函数唯一的情况下，允许用户简写参数，但要返回来让用户确认 TODO
	struct RemoveFuncCmd
	{
		JsonObject ProcessArg(string_view args)
		{
			auto funcInfo = args;
			auto type = GetFuncTypeFrom(funcInfo);
			auto j = Json::JsonConverter::Serialize(RemoveFuncRequest::Content{ type });
			return j;
		}

		vector<string> ProcessResponse(string_view response)
		{
			HandleOperationResponse<void>(response);
			return { SuccessTip };
		}
	};

	struct SearchFuncCmd
	{
		JsonObject ProcessArg(string_view args)
		{
			auto keyword = args;
			return Serial(CombineTo<SearchFuncRequest::Content>(string(keyword)));
		}

		vector<string> ProcessResponse(string_view response)
		{
			auto funcTypeKeySummarys = HandleOperationResponse<decltype(declval<SearchFuncRequest>().Result)>(response);
			vector<string> displayLines;
			for (auto& p : funcTypeKeySummarys)
			{
				displayLines.push_back(move(p.first));
			}

			return displayLines;
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
		JsonObject ProcessArg(string_view args)
		{
			auto [funcInfo, newPackageInfo] = DivideInfo(args);

			auto func = GetFuncTypeFrom(funcInfo);
			auto newPackage = GetPackageFrom(newPackageInfo);

			return Serial(CombineTo<ModifyFuncPackageRequest::Content>(move(func), move(newPackage)));
		}

		vector<string> ProcessResponse(string_view response)
		{
			HandleOperationResponse<void>(response);
			return { SuccessTip };
		}
	};

	struct ContainsFuncCmd
	{
		JsonObject ProcessArg(string_view args)
		{
			auto funcInfo = args;
			return Serial(CombineTo<ContainsFuncRequest::Content>(GetFuncTypeFrom(funcInfo)));
		}

		vector<string> ProcessResponse(string_view response)
		{
			return { HandleOperationResponse<bool>(response) ? "true" : "false", };
		}
	};

	struct GetFuncsInfoCmd
	{
		/// no args need to process
		vector<string> ProcessResponse(string_view response)
		{
			auto funcTypes = HandleOperationResponse<decltype(declval<GetFuncsInfoRequest>().Result)>(response);
			auto header = PredefineHeader::NewFrom(move(funcTypes));

			auto g = header.GetHeaderDef();
			{
				ofstream f("Predefine.hpp");
				while (g.MoveNext())
				{
					f << g.Current() << std::endl;
				}
			}

			return { "Predefine.hpp generated!" };
		}
	};

	struct ShutdownCmd
	{
		/// no args need to process
		vector<string> ProcessResponse(string_view response)
		{
			HandleOperationResponse<void>(response);
			return { "Server will shutdown in 30s!" };
		}
	};

	struct AddClientAccountCmd
	{
		JsonObject ProcessArg(string_view args)
		{
			auto accountInfo = args;
			auto [username, password] = ParseOut<true>(accountInfo, " ");
			return Serial(CombineTo<AddClientAccountRequest::Content>(string(username), string(password)));
		}

		vector<string> ProcessResponse(string_view response)
		{
			HandleOperationResponse<void>(response);
			return { SuccessTip };
		}
	};

	struct RemoveClientAccountCmd
	{
		JsonObject ProcessArg(string_view args)
		{
			auto username = args;
			return Serial(CombineTo<RemoveClientAccountRequest::Content>(string(username)));
		}

		vector<string> ProcessResponse(string_view response)
		{
			HandleOperationResponse<void>(response);
			return { SuccessTip };
		}
	};

	struct AddAdminAccountCmd
	{
		JsonObject ProcessArg(string_view args)
		{
			auto accountInfo = args;
			auto [username, password] = ParseOut<true>(accountInfo, " ");
			return Serial(CombineTo<AddAdminAccountRequest::Content>(string(username), string(password)));
		}

		vector<string> ProcessResponse(string_view response)
		{
			HandleOperationResponse<void>(response);
			return { SuccessTip };
		}
	};

	struct RemoveAdminAccountCmd
	{
		JsonObject ProcessArg(string_view args)
		{
			auto username = args;
			return Serial(CombineTo<RemoveClientAccountRequest::Content>(string(username)));
		}

		vector<string> ProcessResponse(string_view response)
		{
			HandleOperationResponse<void>(response);
			return { SuccessTip };
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

	template <typename T>
	concept HasProcess = requires(T t, string_view s)
	{
		t.ProcessArg(s);
	};

	/// return value: requests and response processor
	pair<vector<string>, function<vector<string>(string_view)>> ProcessCmd(string const& cmd)
	{
		auto [cmdName, remain] = ParseOut<false>(TrimStart(cmd), " ");
		auto args = TrimEnd(TrimStart(remain));
		vector<string> requests;
		function<vector<string>(string_view)> responseProcessor;

		switch (StrToInt(cmdName))
		{
#define CASE_OF(NAME)                                                                 \
	case StrToInt(nameof(NAME)):                                                      \
		requests.push_back(Json::JsonConverter::Serialize(Network::NAME).ToString()); \
		{                                                                             \
			auto c = NAME##Cmd();                                                     \
			requests.push_back(c.ProcessArg(args).ToString());                        \
			responseProcessor = [c = move(c)](string_view response) mutable           \
			{                                                                         \
				return c.ProcessResponse(response);                                   \
			};                                                                        \
		}                                                                             \
		break;

			CASE_OF(AddFunc);
			CASE_OF(RemoveFunc);
			CASE_OF(SearchFunc);
			CASE_OF(ModifyFuncPackage);
			CASE_OF(ContainsFunc);
			CASE_OF(AddClientAccount);
			CASE_OF(AddAdminAccount);
			CASE_OF(RemoveClientAccount);
			CASE_OF(RemoveAdminAccount);
		case StrToInt(nameof(GetFuncsInfo)):
			requests.push_back(Json::JsonConverter::Serialize(Network::GetFuncsInfo).ToString());
			{
				auto c = GetFuncsInfoCmd();
				responseProcessor = [c = move(c)](string_view response) mutable
				{
					return c.ProcessResponse(response);
				};
			}
			break;
		case StrToInt(nameof(Shutdown)):
			requests.push_back(Json::JsonConverter::Serialize(Network::Shutdown).ToString());
			{
				auto c = ShutdownCmd();
				responseProcessor = [c = move(c)](string_view response) mutable
				{
					return c.ProcessResponse(response);
				};
			}
			break;
		default: throw invalid_argument(string("No handler of ").append(cmd));

#undef CASE_OF
		}

		return pair(move(requests), move(responseProcessor));
	}
#undef nameof
}

#pragma once
#include <type_traits>
#include "ServiceBase.hpp"
#include "promise_Void.hpp"

#include <vector>
#include <string>

namespace Server
{
	using ::std::string;
	using ::std::vector;

	enum ServiceOption
	{
		AddFunc,
		RemoveFunc,
		SearchFunc,
		ModifyFunc,
	};

	// 这里的这些类与 Worker 文件里的类有重复，之后考虑消除
	struct AddFuncInfo
	{
		vector<string> Package;
		string FuncsDef;
		string Summary;
	};

	struct RemoveFuncInfo
	{

	};

	struct SearchFuncInfo
	{

	};

	struct ModifyFuncInfo
	{

	};
}

namespace Json::JsonConverter
{
	using Server::ServiceOption;

#define nameof(VAR) #VAR
	template <>
	JsonObject Serialize(ServiceOption const& option)
	{
		return Serialize(static_cast<int>(option));
	}

	template <>
	ServiceOption Deserialize(JsonObject const& jsonObj)
	{
		return static_cast<ServiceOption>(Deserialize<int>(jsonObj));
	}
#undef nameof
}

namespace Server
{
	using ::std::is_same_v;
	using ::std::move;
	using namespace Json;

	class AdminService : private ServiceBase
	{
	private:
		using Base = ServiceBase;

	public:
		using Base::Base;

		void Run()
		{
			auto AddFunc = [this]() -> Void
			{
				string response;

				// TODO add arg
				co_await _funcLibWorker->AddFunc({}, {{}}, "Hello");
				_responder->RespondTo(_peer, response);

			};

			auto RemoveFunc = [&]()
			{

			};

			auto SearchFunc = [&]() -> Void
			{
				JsonObject arg;
				string response;
#define nameof(VAR) #VAR

				if constexpr (is_same_v<decltype(_funcLibWorker->SearchFunc(JsonConverter::Deserialize<string>(arg))), void>)
				{
					// co_await _funcLibWorker->SearchFunc(JsonConverter::Deserialize<string>(arg));
					response = nameof(SearchFunc) " operate succeed";
				}
				else
				{
					// auto result = _funcLibWorker->SearchFunc(JsonConverter::Deserialize<string>(arg));
					// auto json = JsonConverter::Serialize(result);
					// response = json.ToString();
				}
#undef nameof
				_responder->RespondTo(_peer, response);
				return {}; // 这句是为了消除编译 warning，之后使用协程后可以去掉
			};

			auto ModifyFunc = []{};

			LoopAcquireThenDispatch<ServiceOption>(
				[this](auto serviceOption)
				{
					return static_cast<int>(serviceOption);
				},
				AddFunc,
				RemoveFunc,
				SearchFunc,
				ModifyFunc
				// 这里的 lambda 里面还可以用 LoopAcquireThenDispatch，这样就可以写复杂情况了
			);
		}
	};
}

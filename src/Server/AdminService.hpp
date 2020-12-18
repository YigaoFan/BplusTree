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
#define HANDLER_OF(NAME, ARG_TYPE)                                          \
	auto NAME = [this]() -> Void                                            \
	{                                                                       \
		string response;                                                    \
		auto arg = ReceiveFromClient<ARG_TYPE>();                           \
		if constexpr (is_same_v<decltype(_funcLibWorker->NAME(arg)), void>) \
		{                                                                   \
			co_await _funcLibWorker->NAME(move(arg));                       \
			response = nameof(NAME) " operate succeed(或者返回 null 也行)";                     \
		}                                                                   \
		else                                                                \
		{                                                                   \
			auto result = co_await _funcLibWorker->NAME(move(arg));         \
			response = Json::JsonConverter::Serialize(result);              \
		}                                                                   \
		_responder->RespondTo(_peer, response);                             \
	}
#define nameof(VAR) #VAR

#undef nameof
			auto AddFunc = [this]() -> Void 
			{
				// TODO add arg
				co_await _funcLibWorker->AddFunc({}, {{}}, "Hello");
				_responder->RespondTo(_peer, "remove succeed");
			};

			auto RemoveFunc = [this]() -> Void
			{
				auto func = ReceiveFromClient<FuncType>();
				co_await _funcLibWorker->RemoveFunc(move(func));
			};

			auto SearchFunc = [this]() -> Void
			{

				auto keyword = ReceiveFromClient<string>();
				auto result = co_await _funcLibWorker->SearchFunc(move(keyword));
				auto response = Json::JsonConverter::Serialize(result).ToString();
				_responder->RespondTo(_peer, response);
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

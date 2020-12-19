#pragma once
#include <type_traits>
#include "ServiceBase.hpp"
#include "promise_Void.hpp"

namespace Server
{
	enum ServiceOption
	{
		AddFunc,
		RemoveFunc,
		SearchFunc,
		ModifyFuncPackage,
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

#define PRIMITIVE_CAT(A, B) A##B
#define CAT(A, B) PRIMITIVE_CAT(A, B)
		void Run()
		{
// 后面看有没有办法利用模板运算把两个宏合为一个，难点在于 co_await 那一步和 result 很紧密，而 result 关系到 response
// 其实就是统一返回值类型为 string
// 以及生成 response 那一步可以拿个函数包一下，这样可以进行一些调整操作
#define ASYNC_HANDLER_OF_NO_RESULT(NAME)                                   \
	[this]() -> Void                                                       \
	{                                                                      \
		auto paras = ReceiveFromClient<CAT(NAME, Request)::Content>();     \
		co_await _funcLibWorker->NAME(move(paras));                        \
		auto response = nameof(NAME) " operate succeed(或者返回 null 也行)"; \
		_responder->RespondTo(_peer, response);                            \
	}

#define ASYNC_HANDLER_OF_WITH_RESULT(NAME)                                 \
	[this]() -> Void                                                       \
	{                                                                      \
		auto paras = ReceiveFromClient<CAT(NAME, Request)::Content>();     \
		auto result = co_await _funcLibWorker->NAME(move(paras));          \
		auto response = Json::JsonConverter::Serialize(result).ToString(); \
		_responder->RespondTo(_peer, response);                            \
	}
#define nameof(VAR) #VAR

			auto ModifyFunc = []{};

			LoopAcquireThenDispatch<ServiceOption>(
				[this](auto serviceOption)
				{
					return static_cast<int>(serviceOption);
				},
				ASYNC_HANDLER_OF_NO_RESULT(AddFunc),
				ASYNC_HANDLER_OF_NO_RESULT(RemoveFunc),
				ASYNC_HANDLER_OF_WITH_RESULT(SearchFunc),
				ASYNC_HANDLER_OF_NO_RESULT(ModifyFuncPackage)
				// 这里的 lambda 里面还可以用 LoopAcquireThenDispatch，这样就可以写复杂情况了
			);
#undef nameof
		}
// #undef CAT 因为 FlyTest 也用这个，所以不能 undef，之后把部分代码移入 cpp 文件应该可以 undef 了
// #undef PRIMITIVE_CAT
	};
}

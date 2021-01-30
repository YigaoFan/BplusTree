#pragma once
#include "ServiceBase.hpp"
#include "AccountManager.hpp"

namespace Server
{
	enum ServiceOption
	{
		AddFunc,
		RemoveFunc,
		SearchFunc,
		ModifyFuncPackage,
		ContainsFunc,
		AddClientAccount,
		RemoveClientAccount,
		AddAdminAccount,
		RemoveAdminAccount,
	};
}

namespace Json::JsonConverter
{
	using Server::ServiceOption;

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
}

namespace Server
{
	class AdminService : private ServiceBase
	{
	private:
		using Base = ServiceBase;
		AccountManager* _accountManager;

	public:
		AdminService(shared_ptr<Socket> peer, FuncLibWorker* funcLibWorker, Responder* responder, AccountManager* accountManager)
			: Base(move(peer), funcLibWorker, responder), _accountManager(accountManager)
		{ }

// 去掉这个宏 TODO
#define ASYNC_ACCOUNT_HANDLER(NAME)                                                                                \
	[responder = _responder, peer = _peer, accountManager = _accountManager](auto userLoggerPtr) mutable -> Void   \
	{                                                                                                              \
		auto [paras, rawStr] = ReceiveFromPeer<CAT(NAME, Request)::Content, true>(peer.get());                     \
		auto id = GenerateRequestId();                                                                             \
		auto idLogger = userLoggerPtr->BornNewWith(id);                                                            \
		responder->RespondTo(peer, id);                                                                            \
		auto requestLogger = idLogger.BornNewWith(nameof(NAME));                                                   \
		auto argLogger = requestLogger.BornNewWith(move(rawStr));                                                  \
		string response;                                                                                           \
		try                                                                                                        \
		{                                                                                                          \
			if constexpr (not is_same_v<void, decltype(accountManager->NAME(move(paras)).await_resume())>)         \
			{                                                                                                      \
				auto result = co_await accountManager->NAME(move(paras));                                          \
				response = Json::JsonConverter::Serialize(result).ToString();                                      \
			}                                                                                                      \
			else                                                                                                   \
			{                                                                                                      \
				co_await accountManager->NAME(move(paras));                                                        \
				response = nameof(NAME) " operate succeed(or TODO null)";                                          \
			}                                                                                                      \
		}                                                                                                          \
		catch (std::exception const &e)                                                                            \
		{                                                                                                          \
			responder->RespondTo(peer, e.what());                                                                  \
			argLogger.BornNewWith(ResultStatus::Failed);                                                           \
			co_return;                                                                                             \
		}                                                                                                          \
		responder->RespondTo(peer, response);                                                                      \
		argLogger.BornNewWith(ResultStatus::Complete);                                                             \
	}
		void Run(auto userLogger)
		{
			// 那出异常怎么设置失败呢
			// 这里 LoopAcquireThenDispatch 用 userLogger 来记录这里面的日志
			// 各个 handler 通过 capture 自行处理会产生的各自的 requestLogger
			// 这个产生了不必要的 requestLogger，这点之后应该要处理
			AsyncLoopAcquireThenDispatch<ServiceOption>(
				move(userLogger),
				_peer,
				[](auto serviceOption) {
					return static_cast<int>(serviceOption);
				},
				// 下面的顺序需要和 ServiceOption enum 的顺序一致
				ASYNC_HANDLER(AddFunc),
				ASYNC_HANDLER(RemoveFunc),
				ASYNC_HANDLER(SearchFunc),
				ASYNC_HANDLER(ModifyFuncPackage),
				ASYNC_HANDLER(ContainsFunc),
				ASYNC_ACCOUNT_HANDLER(AddClientAccount),
				ASYNC_ACCOUNT_HANDLER(RemoveClientAccount),
				ASYNC_ACCOUNT_HANDLER(AddAdminAccount),
				ASYNC_ACCOUNT_HANDLER(RemoveAdminAccount));
		}
	};
}

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

#define ASYNC_ACCOUNT_MANAGE_HANDLER(NAME) ASYNC_HANDLER(NAME, _accountManager)
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
				ASYNC_FUNC_LIB_HANDLER(AddFunc),
				ASYNC_FUNC_LIB_HANDLER(RemoveFunc),
				ASYNC_FUNC_LIB_HANDLER(SearchFunc),
				ASYNC_FUNC_LIB_HANDLER(ModifyFuncPackage),
				ASYNC_FUNC_LIB_HANDLER(ContainsFunc),
				ASYNC_ACCOUNT_MANAGE_HANDLER(AddClientAccount),
				ASYNC_ACCOUNT_MANAGE_HANDLER(RemoveClientAccount),
				ASYNC_ACCOUNT_MANAGE_HANDLER(AddAdminAccount),
				ASYNC_ACCOUNT_MANAGE_HANDLER(RemoveAdminAccount));
		}
	};
}

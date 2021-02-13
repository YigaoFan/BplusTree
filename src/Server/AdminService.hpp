#pragma once
#include <functional>
#include "ServiceBase.hpp"
#include "AccountManager.hpp"

namespace Server
{
	using Network::AdminServiceOption;
	using ::std::function;

	class AdminService : private ServiceBase
	{
	private:
		using Base = ServiceBase;
		AccountManager* _accountManager;
		function<void()> _shutdown;

	public:
		AdminService(shared_ptr<Socket> peer, FuncLibWorker* funcLibWorker, Responder* responder, AccountManager* accountManager, function<void()> shutdown)
			: Base(move(peer), funcLibWorker, responder), _accountManager(accountManager), _shutdown(move(shutdown))
		{ }

#define ASYNC_ACCOUNT_MANAGE_HANDLER(NAME) ASYNC_HANDLER(NAME, _accountManager)
		void Run(auto userLogger)
		{
			auto shutdown = [s=move(_shutdown), peer=_peer,responder=_responder](auto userLoggerPtr) -> Void
			{
				auto id = GenerateRequestId();
				auto idLogger = userLoggerPtr->BornNewWith(id);
				responder->RespondTo(peer, id);
				auto requestLogger = idLogger.BornNewWith(nameof(Shutdown));
				auto argLogger = requestLogger.BornNewWith(nameof(-));
				JsonObject result;
				s();
				ReturnToPeer(responder, peer, move(result));
				argLogger.BornNewWith(ResultStatus::Complete);
				co_return;
			};
			AsyncLoopAcquireThenDispatch<AdminServiceOption>(
				move(userLogger),
				_peer,
				[](auto serviceOption) { return static_cast<int>(serviceOption); },
				// 下面的顺序要和 AdminServiceOption enum 中定义顺序一致
				ASYNC_FUNC_LIB_HANDLER(AddFunc),
				ASYNC_FUNC_LIB_HANDLER(RemoveFunc),
				ASYNC_FUNC_LIB_HANDLER(SearchFunc),
				ASYNC_FUNC_LIB_HANDLER(ModifyFuncPackage),
				ASYNC_FUNC_LIB_HANDLER(ContainsFunc),
				ASYNC_ACCOUNT_MANAGE_HANDLER(AddClientAccount),
				ASYNC_ACCOUNT_MANAGE_HANDLER(RemoveClientAccount),
				ASYNC_ACCOUNT_MANAGE_HANDLER(AddAdminAccount),
				ASYNC_ACCOUNT_MANAGE_HANDLER(RemoveAdminAccount),
				ASYNC_HANDLER_WITHOUT_ARG(GetFuncsInfo, _funcLibWorker),
				move(shutdown)
				);
		}
	};
}

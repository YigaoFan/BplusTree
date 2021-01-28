#pragma once
#include <tuple>
#include <memory>
#include <utility>
#include <type_traits>
#include "FuncLibWorker.hpp"
#include "Responder.hpp"
#include "RequestId.hpp"
#include "Util.hpp"
#include "promise_Void.hpp"
#include "../TestFrame/Util.hpp" // TODO remove
using namespace Test;

namespace Server
{
	using ::std::is_same_v;
	using ::std::pair;
	using ::std::shared_ptr;
	using ::std::tuple;
	using ::std::tuple_size_v;

	class ServiceBase
	{
	protected:
		shared_ptr<Socket> _peer;
		FuncLibWorker* _funcLibWorker;
		Responder* _responder;

	public:
		ServiceBase(shared_ptr<Socket> peer, FuncLibWorker* funcLibWorker, Responder* responder)
			: _peer(move(peer)), _funcLibWorker(funcLibWorker), _responder(responder)
		{ }
	
	protected:
		// 外面使用这个函数的地方是不是要像下下面拿个函数那样使用异常处理？ TODO
		template <typename Return,
				  bool ReturnAdditionalRawByte = false,
				  ByteProcessWay ByteProcessWay = ByteProcessWay::ParseThenDeserial,
				  typename Logger>
		static auto ReceiveFromClient(Logger* logger, Socket* peer)
		{
			return Receive<Return, ReturnAdditionalRawByte, ByteProcessWay>(peer);
		}

		template <typename Callback>
		static Void Loop(Callback callback)
		{
			// log and send and throw
			for (;;)
			{
				co_await callback(); // 这一步要不要异常处理？
				printf("coroutine come back here\n");
			}
		}

		template <typename Receive, typename UserLogger, typename Dispatcher, typename... Handlers>
		static void LoopAcquireThenDispatch(UserLogger userLogger, Dispatcher dispatcher, Handlers... handlers)
		{
			for (;;)
			{
				// tuple handlersTuple = { move(handlers)...};
				// auto r = ReceiveFromClient<Receive>(&userLogger);
				// auto d = dispatcher(move(r));

				// InvokeWhenEqualTo<0>(d, move(handlersTuple));
			}
		}

		template <auto CurrentOption, typename... Handlers>
		static void InvokeWhenEqualTo(int dispatcherResult, tuple<Handlers...> handlersTuple)
		{
			if (dispatcherResult == CurrentOption)
			{
				std::get<CurrentOption>(handlersTuple)();
			}
			else if constexpr ((CurrentOption + 1) < tuple_size_v<decltype(handlersTuple)>)
			{
				InvokeWhenEqualTo<CurrentOption + 1>(dispatcherResult, move(handlersTuple));
			}
			else
			{
				throw string("No handler of ") + std::to_string(dispatcherResult);
			}
		}

#define nameof(VAR) #VAR
#define PRIMITIVE_CAT(A, B) A##B
#define CAT(A, B) PRIMITIVE_CAT(A, B)

#define ASYNC_HANDLER(NAME)                                                                                          \
	[responder=_responder, libWorker = _funcLibWorker, peer = _peer, userLogger = move(userLogger)]() mutable -> Void\
	{                                                                                                                \
		auto id = GenerateRequestId();                                                                               \
		auto idLogger = userLogger.BornNewWith(id);                                                                  \
		responder->RespondTo(peer, id);                                                                             \
		auto requestLogger = idLogger.BornNewWith(nameof(NAME));                                                     \
		auto [paras, rawStr] = ReceiveFromClient<CAT(NAME, Request)::Content, true>(&requestLogger, peer.get());                 \
		auto argLogger = requestLogger.BornNewWith(move(rawStr));                                                    \
		string response;                                                                                             \
		if constexpr (not is_same_v<void, decltype(libWorker->NAME(move(paras)).await_resume())>)                    \
		{                                                                                                            \
			auto result = co_await libWorker->NAME(move(paras));                                                     \
			response = Json::JsonConverter::Serialize(result).ToString();                                            \
		}                                                                                                            \
		else                                                                                                         \
		{                                                                                                            \
			co_await libWorker->NAME(move(paras));                                                                   \
			response = nameof(NAME) " operate succeed(or TODO null)";                                                \
		}                                                                                                            \
		responder->RespondTo(peer, response);                                                                       \
		argLogger.BornNewWith(ResultStatus::Complete);                                                               \
	}
	};
}

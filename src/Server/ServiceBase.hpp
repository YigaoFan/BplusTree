#pragma once
#include <tuple>
#include <memory>
#include <utility>
#include <type_traits>
#include "FuncLibWorker.hpp"
#include "Responder.hpp"
#include "RequestId.hpp"
#include "Util.hpp"
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
		auto ReceiveFromClient(Logger* logger)
		{
			return Receive<Return, ReturnAdditionalRawByte, ByteProcessWay>(_peer.get());
		}

		template <typename Callback>
		void Loop(Callback callback)
		{
			// log and send and throw

			while (true)
			{
				callback(); // 这一步要不要异常处理？
			}
		}

		template <typename Receive, typename UserLogger, typename Dispatcher, typename... Handlers>
		void LoopAcquireThenDispatch(UserLogger userLogger, Dispatcher dispatcher, Handlers... handlers)
		{
			while (true)
			{
				tuple handlersTuple = { move(handlers)...};
				// auto r = try_with_exception_handle(ReceiveFromClient<Receive>(&userLogger));
				auto r = ReceiveFromClient<Receive>(&userLogger);
				auto d = dispatcher(move(r));

				InvokeWhenEqualTo<0>(d, move(handlersTuple));
			}
		}

		template <auto CurrentOption, typename... Handlers>
		void InvokeWhenEqualTo(int dispatcherResult, tuple<Handlers...> handlersTuple)
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

#define ASYNC_HANDLER(NAME)                                                                            \
	[this, userLogger = move(userLogger)]() mutable -> Void                                            \
	{                                                                                                  \
		auto id = GenerateRequestId();                                                                 \
		auto idLogger = userLogger.BornNewWith(id);                                                    \
		_responder->RespondTo(_peer, id);                                                              \
		auto requestLogger = idLogger.BornNewWith(nameof(NAME));                                       \
		auto [paras, rawStr] = ReceiveFromClient<CAT(NAME, Request)::Content, true>(&requestLogger);   \
		auto argLogger = requestLogger.BornNewWith(move(rawStr));                                      \
		string response;                                                                               \
		if constexpr (not is_same_v<void, decltype(_funcLibWorker->NAME(move(paras)).await_resume())>) \
		{                                                                                              \
			auto result = co_await _funcLibWorker->NAME(move(paras));                                  \
			response = Json::JsonConverter::Serialize(result).ToString();                              \
		}                                                                                              \
		else                                                                                           \
		{                                                                                              \
			co_await _funcLibWorker->NAME(move(paras));                                                \
			response = nameof(NAME) " operate succeed(or TODO null)";                                  \
		}                                                                                              \
		_responder->RespondTo(_peer, response);                                                        \
		argLogger.BornNewWith(ResultStatus::Complete);                                                 \
	}
	};
}

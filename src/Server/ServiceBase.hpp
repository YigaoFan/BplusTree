#pragma once
#include <tuple>
#include <memory>
#include <type_traits>
#include "RequestId.hpp"
#include "../Json/JsonConverter/JsonConverter.hpp"
#include "FuncLibWorker.hpp"
#include "Responder.hpp"
#include "RequestId.hpp"
#include "Util.hpp"
#include "AwaiterVoid.hpp"

namespace Server
{
	using Log::ResultStatus;
	using ::std::is_same_v;
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
		template <typename Return,
				  bool ReturnAdditionalRawByte = false,
				  ByteProcessWay ByteProcessWay = ByteProcessWay::ParseThenDeserial>
		static auto ReceiveFromClient(Socket* peer)
		{
			return Receive<Return, ReturnAdditionalRawByte, ByteProcessWay>(peer);
		}

		template <typename Callback>
		static Void AsyncLoop(Callback callback)
		{
			for (;;)
			{
				co_await callback();
			}
		}

		template <typename Receive, typename Dispatcher, typename... Handlers>
		static Void AsyncLoopAcquireThenDispatch(shared_ptr<Socket> peer, Dispatcher dispatcher, Handlers... handlers)
		{
			tuple handlersTuple = { move(handlers)...};
			for (;;)
			{
				auto r = ReceiveFromClient<Receive>(peer.get());
				auto d = dispatcher(move(r));
				co_await InvokeWhenEqual(d, move(handlersTuple));
			}
		}

		template <auto CurrentOption = 0, typename... Handlers>
		static auto InvokeWhenEqual(int dispatcherResult, tuple<Handlers...> handlersTuple)
		{
			if (dispatcherResult == CurrentOption)
			{
				return std::get<CurrentOption>(handlersTuple)();
			}
			else if constexpr ((CurrentOption + 1) < tuple_size_v<decltype(handlersTuple)>)
			{
				return InvokeWhenEqual<CurrentOption + 1>(dispatcherResult, move(handlersTuple));
			}
			else
			{
				throw string("No handler of ") + std::to_string(dispatcherResult);
			}
		}

#define nameof(VAR) #VAR
#define PRIMITIVE_CAT(A, B) A##B
#define CAT(A, B) PRIMITIVE_CAT(A, B)
// 以上两个不能 undef，因为 FlyTest 里面也定义了
#define ASYNC_HANDLER(NAME)                                                                                               \
	[responder = _responder, libWorker = _funcLibWorker, peer = _peer, userLogger = move(userLogger)]() mutable -> Void   \
	{                                                                                                                     \
		auto [paras, rawStr] = ReceiveFromClient<CAT(NAME, Request)::Content, true>(peer.get());                          \
		auto id = GenerateRequestId();                                                                                    \
		auto idLogger = userLogger.BornNewWith(id);                                                                       \
		responder->RespondTo(peer, id);                                                                                   \
		auto requestLogger = idLogger.BornNewWith(nameof(NAME));                                                          \
		auto argLogger = requestLogger.BornNewWith(move(rawStr));                                                         \
		string response;                                                                                                  \
		try                                                                                                               \
		{                                                                                                                 \
			if constexpr (not is_same_v<void, decltype(libWorker->NAME(move(paras)).await_resume())>)                     \
			{                                                                                                             \
				auto result = co_await libWorker->NAME(move(paras));                                                      \
				response = Json::JsonConverter::Serialize(result).ToString();                                             \
			}                                                                                                             \
			else                                                                                                          \
			{                                                                                                             \
				co_await libWorker->NAME(move(paras));                                                                    \
				response = nameof(NAME) " operate succeed(or TODO null)";                                                 \
			}                                                                                                             \
		}                                                                                                                 \
		catch (std::exception const& e)                                                                                   \
		{                                                                                                                 \
			responder->RespondTo(peer, "task failed");                                                                    \
			argLogger.BornNewWith(ResultStatus::Failed);                                                                  \
			co_return;                                                                                                    \
		}                                                                                                                 \
		responder->RespondTo(peer, response);                                                                             \
		argLogger.BornNewWith(ResultStatus::Complete);                                                                    \
	}
	};
}

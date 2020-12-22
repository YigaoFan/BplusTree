#pragma once
#include <array>
#include <tuple>
#include <memory>
#include <utility>
#include <type_traits>
#include <string_view>
#include "BasicType.hpp"
#include "FuncLibWorker.hpp"
#include "Responder.hpp"
#include "RequestId.hpp"

namespace Server
{
	using ::std::array;
	using ::std::is_same_v;
	using ::std::pair;
	using ::std::shared_ptr;
	using ::std::string_view;
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
		template <typename Return, bool ReturnRawByte = false, typename Logger>
		auto ReceiveFromClient(Logger* logger)
		{
			array<char, 256> buff;
			asio::error_code error;
			auto n = _peer->read_some(asio::buffer(buff), error);

			if (error)
			{
				// 抛异常不要紧，重要的是在抛之前把异常 log 下来
				string message;
				if (error == asio::error::eof)
				{
					message = string("Client disconnect: " + error.message());
				}
				else
				{
					message = string("Read from client error: " + error.message());
				}

				logger->Error(message);
				throw ;// TODO exception
			}
			else
			{
				auto input = string_view(buff.data(), n);
				try
				{
					auto jsonObj = Json::Parse(input);
					auto ret = Json::JsonConverter::Deserialize<Return>(jsonObj);
					if constexpr (ReturnRawByte)
					{
						return pair<Return, string>(move(ret), string(input));
					}
					else
					{
						return ret;
					}
				}
				catch (std::exception const& e)
				{
					logger->Error("Parse client content or deserialize error: ", e);
					throw e;
				}
			}
		}

// TODO wait complete
#define try_with_exception_handle(STATEMENT) [&] { \
	try                                            \
	{                                              \
		return STATEMENT;                          \
	}                                              \
	catch (...)                                    \
	{                                              \
		throw;                                     \
	}                                              \
}()

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
			// 或许可以允许差量提供 handler？最后一个作为 default？
			while (true)
			{
				tuple handlersTuple = { move(handlers)...};
				auto r = try_with_exception_handle(ReceiveFromClient<Receive>(&userLogger));
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
#undef try_with_exception_handle

#define nameof(VAR) #VAR

#define ASYNC_HANDLER(NAME)                                                                            \
	[this, idLogger = userLogger.BornNewWith(GenerateRequestId())]() mutable -> Void                   \
	{                                                                                                  \
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

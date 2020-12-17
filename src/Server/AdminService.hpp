#pragma once
#include <memory>
#include <type_traits>
#include <utility>
#include <tuple>
#include "BasicType.hpp"
#include "Responder.hpp"
#include "FuncLibWorker.hpp"
#include "promise_Void.hpp"

namespace Server
{
	using ::std::is_same_v;
	using ::std::move;
	using ::std::pair;
	using ::std::shared_ptr;
	using ::std::tuple;
	using namespace Json;

	class AdminService
	{
	private:
		shared_ptr<Socket> _peer;
		FuncLibWorker* _funcLibWorker;
		Responder* _responder;

	public:
		AdminService(shared_ptr<Socket> peer, FuncLibWorker* funcLibWorker, Responder* responder)
			: _peer(move(peer)), _funcLibWorker(funcLibWorker), _responder(responder)
		{ }

		enum ServiceOption
		{
			AddFunc,
			RemoveFunc,
			SearchFunc,
			End,
			// ModifyFunc,
		};

		void Run()
		{
			auto AddFunc = [&]() -> Void
			{
				string response;

				// TODO add arg
				co_await _funcLibWorker->AddFunc({}, {{}}, "Hello");
				_responder->RespondTo(_peer, response);

			};

			auto RemoveFunc = [&]()
			{

			};

			auto SearchFunc = [&](JsonObject arg) -> Void
			{
				string response;
#define nameof(VAR) #VAR

				if constexpr (is_same_v<decltype(_funcLibWorker->SearchFunc(JsonConverter::Deserialize<string>(arg))), void>)
				{
					// co_await _funcLibWorker->SearchFunc(JsonConverter::Deserialize<string>(arg));
					response = nameof(SearchFunc) " operate succeed";
				}
				else
				{
					// try
					// {

					// }
					// catch ()
					// {

					// }

					// auto result = _funcLibWorker->SearchFunc(JsonConverter::Deserialize<string>(arg));
					// auto json = JsonConverter::Serialize(result);
					// response = json.ToString();
				}
#undef nameof
				_responder->RespondTo(_peer, response);
				return {}; // 这句是为了消除编译 warning，之后使用协程后可以去掉
			};

			while (true)
			{

				ServiceOption op = static_cast<ServiceOption>(ReceiveFromClient<int>());

#define PROCESS(OP_NAME, ...)    \
	case ServiceOption::OP_NAME: \
		OP_NAME(__VA_ARGS__);    \
		break

				switch (op)
				{
					PROCESS(AddFunc);
					PROCESS(RemoveFunc);
					PROCESS(SearchFunc, {});
				default:
					return;
				}
#undef PROCESS
			}
		}

	private:
		template <typename Return>
		Return ReceiveFromClient()
		{
			array<char, 64> buff;
			asio::error_code error;
			auto n = _peer->read_some(asio::buffer(buff), error);

			if (error)
			{
				if (error == asio::error::eof)
				{
					// log client disconnect
				}
				else
				{
					// log message
				}
				
				// TODO modify
				throw string("Access end due to " + error.message());
			}
			else
			{
				auto input = string_view(buff.data(), n);
				auto jsonObj = Json::Parse(input);
				return JsonConverter::Deserialize<Return>(jsonObj);
			}
		}

#define try_with_exception_handle(STATEMENT) [&] { \
	try                                            \
	{                                              \
		return STATEMENT;                          \
	}                                              \
	catch (...)                                    \
	{                                              \
	}                                              \
}()
		template <typename Receive, typename Handler, typename... Handlers>
		void Fun(Handler handler)
		{
			// log and send and throw 

			while (true)
			{
				auto r = try_with_exception_handle(ReceiveFromClient<Receive>());
				try_with_exception_handle(handler(move(r)));
			}
#undef try_with_exception_handle
		}

		template <typename Receive, typename Dispatcher, typename... Handlers>
		void Fun(Dispatcher dispatcher, Handlers... handlers)
		{

			while (true)
			{
				tuple handlersTuple = { move(handlers)... };
				auto r = try_with_exception_handle(ReceiveFromClient<Receive>());
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
			else if constexpr (CurrentOption < std::tuple_size_v<decltype(handlersTuple)>)
			{
				InvokeWhenEqualTo(dispatcherResult, move(handlersTuple));
			}
			else
			{
				throw string("No handler of ") + std::to_string(dispatcherResult);
			}
		}
	};
}

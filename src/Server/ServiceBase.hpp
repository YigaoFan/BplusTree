#pragma once
#include <array>
#include <tuple>
#include <memory>
#include <functional>
#include <string_view>
#include "BasicType.hpp"
#include "FuncLibWorker.hpp"
#include "Responder.hpp"

namespace Server
{
	using ::std::array;
	using ::std::function;
	using ::std::shared_ptr;
	using ::std::string_view;
	using ::std::tuple;
	using std::tuple_size_v;

	class ServiceBase
	{
	protected:
		shared_ptr<Socket> _peer;
		FuncLibWorker* _funcLibWorker;
		Responder* _responder;
		function<void(string)> _log;// 这个要是支持不定参数就好了

	public:
		ServiceBase(shared_ptr<Socket> peer, FuncLibWorker* funcLibWorker, Responder* responder)
			: _peer(move(peer)), _funcLibWorker(funcLibWorker), _responder(responder)
		{ }
	
	protected:
		// void Log()
		// {
			// client ip: XXX.XXX
			// rfc: -
			// username:
			// timestamp
			// http request 这里可以放请求的功能: -
			// http status code: -
			// requested bytes size 这个在收 request 那里才有意义吧: 
			// refer: 应该也是空 -
			// user agent 应该也没有
		// }
		
		// 使用这个函数的地方是不是要像下下面拿个函数那样使用异常处理？ TODO
		template <typename Return>
		Return ReceiveFromClient()
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

				_log(message);
				throw ;
			}
			else
			{
				auto input = string_view(buff.data(), n);
				try
				{
					auto jsonObj = Json::Parse(input);
					return Json::JsonConverter::Deserialize<Return>(jsonObj);
				}
				catch (std::exception const& e)
				{
					// _log("Parse client content or deserialize error: ", e.what());
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

		template <typename Receive, typename Callback>
		void LoopAcquire(Callback callback)
		{
			// log and send and throw

			while (true)
			{
				auto r = try_with_exception_handle(ReceiveFromClient<Receive>());
				callback(move(r)); // 这一步要不要异常处理？
			}
		}

		template <typename Receive, typename Dispatcher, typename... Handlers>
		void LoopAcquireThenDispatch(Dispatcher dispatcher, Handlers... handlers)
		{
			// 或许可以允许差量提供 handler？最后一个作为 default？
			while (true)
			{
				tuple handlersTuple = { move(handlers)...};
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
	};
}

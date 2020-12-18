#pragma once
#include <array>
#include <tuple>
#include <memory>
#include <string_view>
#include "BasicType.hpp"
#include "FuncLibWorker.hpp"
#include "Responder.hpp"

namespace Server
{
	using ::std::array;
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

	public:
		ServiceBase(shared_ptr<Socket> peer, FuncLibWorker* funcLibWorker, Responder* responder)
			: _peer(move(peer)), _funcLibWorker(funcLibWorker), _responder(responder)
		{ }
	
	protected:
		template <typename Return>
		Return ReceiveFromClient()
		{
			array<char, 256> buff;
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
				return Json::JsonConverter::Deserialize<Return>(jsonObj);
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

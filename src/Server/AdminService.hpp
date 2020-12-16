#pragma once
#include <memory>
#include <type_traits>
#include <utility>
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

				auto op = GetThisSeriveOption();

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
					return;// remember close peer
				}
#undef PROCESS
			}
			_peer->close();
		}

	private:
		ServiceOption GetThisSeriveOption()
		{
			array<char, 64> buff;

			asio::error_code error;
			auto n = _peer->read_some(asio::buffer(buff), error);

			if (error and error == asio::error::eof)
			{
				return ServiceOption::End;
			}
			else
			{
				JsonObject j; // TODO
				return static_cast<ServiceOption>(JsonConverter::Deserialize<int>(j));

			}
		}
	};
}

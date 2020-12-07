#pragma once
#include <functional>
#include <array>
#include <asio.hpp>
#include "../Json/Parser.hpp"
#include "../Json/JsonConverter/JsonConverter.hpp"
#include "BasicType.hpp"
#include "ThreadPool.hpp"
#include "InvokeWorker.hpp"
#include "Responder.hpp"
#include "ClientService.hpp"
#include "AdminService.hpp"

namespace Server
{
	struct LoginInfo
	{
		string Username;
		string Password;
	};
}

namespace Json::JsonConverter
{
	using Server::LoginInfo;

	template <>
	JsonObject Serialize(LoginInfo const& account)
	{
#define nameof(VAR) #VAR
		auto [username, password] = account;
		JsonObject::_Object obj;
		obj.insert({ nameof(username), make_shared<JsonObject>(Serialize(username)) });
		obj.insert({ nameof(password),   make_shared<JsonObject>(Serialize(password)) });

		return JsonObject(move(obj));
	}

	template <>
	LoginInfo Deserialize(JsonObject const& jsonObj)
	{
		auto username = Deserialize<string>(jsonObj[nameof(username)]);
		auto password = Deserialize<string>(jsonObj[nameof(password)]);

		return LoginInfo{ username, password };
	}
#undef nameof
}

namespace Server
{
	using ::std::function;
	using ::std::array;
	using namespace Json;

	class BusinessAcceptor
	{
	private:
		bool _startAccept = false;
		ThreadPool _threadPool;
		Responder _responder;
		function<ClientService(Socket, Responder*)> _clientServiceFactory;
		function<AdminService(Socket, Responder*)> _adminServiceFactory;

	public:
		BusinessAcceptor(ThreadPool threadPool, Responder responder)
			: _threadPool(threadPool), _responder(responder)
		{
		}

		void SetServiceFactory(function<ClientService(Socket, Responder*)> clientServiceFactory,
							function<AdminService(Socket, Responder*)> adminServiceFactory)
		{
			_clientServiceFactory = move(clientServiceFactory);
			_adminServiceFactory = move(adminServiceFactory);
		}

		// asio 的异步服务器示例不是并发处理的吗，而是一个完了接一个的吗？
		void StartAcceptBackground()
		{
			_startAccept = true;
		}

		void HandleAccept(asio::error_code const& error, Socket peer)
		{
			if (not _startAccept)
			{
				return;
			}

			// handle error code TODO

			_threadPool.Execute([p=move(peer), this] () mutable
			{
				CommunicateWith(move(p));
			});
		}

		// peer 是连接中对端的意思
		void CommunicateWith(Socket peer)
		{
			array<char, 128> buff;
			auto size = peer.receive(asio::buffer(buff));

			auto send = [&](string const& message)
			{
				_responder.RespondTo(peer, message);
			};

			try
			{
				// 会不会有多余的空格问题？这个两端发送的时候要统一好
				if (string_view(buff.data(), size) == "hello server")
				{
					send("server ok");
				}
				else
				{
					// error
				}

				auto isAdmin = false;
				// client send username and password 鉴权
				size = peer.receive(asio::buffer(buff));
				auto loginInfoJObj = Json::Parse({ buff.data(), size });
				auto loginInfo = JsonConverter::Deserialize<LoginInfo>(loginInfoJObj);
				
				{
					// search account in account info file
				}

				// 可以 DRY 这种固定的问答模式吗？
				if (isAdmin)
				{
					send("server ok. welcome admin.");
					auto s = _adminServiceFactory(move(peer), &_responder);
					return s.Run();
				}
				else
				{
					send("server ok");
					auto s = _clientServiceFactory(move(peer), &_responder);
					return s.Run();
				}
			}
			catch (...)
			{
				// get info from exception
				send("");
			}
		}
	};
}

#pragma once
#include <array>
#include <memory>
#include <functional>
#include <asio.hpp>
#include "../Json/Parser.hpp"
#include "../Json/JsonConverter/JsonConverter.hpp"
#include "BasicType.hpp"
#include "ThreadPool.hpp"
#include "FuncLibWorker.hpp"
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
	using ::std::array;
	using ::std::function;
	using ::std::shared_ptr;
	using namespace Json;

	class BusinessAcceptor
	{
	private:
		bool _startAccept = false;
		ThreadPool* _threadPool = nullptr;
		Responder _responder;
		function<ClientService(shared_ptr<Socket>, Responder*)> _clientServiceFactory;
		function<AdminService(shared_ptr<Socket>, Responder*)> _adminServiceFactory;

	public:
		BusinessAcceptor(Responder responder)
			: _responder(move(responder))
		{
		}

		void SetThreadPool(ThreadPool* threadPool)
		{
			_threadPool = threadPool;
		}

		template <typename ClientServiceFactory, typename AdminServiceFactory>
		void SetServiceFactory(ClientServiceFactory clientServiceFactory, AdminServiceFactory adminServiceFactory)
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

			if (error)
			{
				
			}
			
			_threadPool->Execute([p=make_shared<Socket>(move(peer)), this] () mutable
			{
				CommunicateWith(move(p));
			});
		}

		// peer 是连接中对端的意思
		void CommunicateWith(shared_ptr<Socket> peer)
		{
			array<char, 128> buff;
			auto size = peer->receive(asio::buffer(buff));

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
				size = peer->receive(asio::buffer(buff));
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
				send("Exception happend");
			}
		}
	};
}

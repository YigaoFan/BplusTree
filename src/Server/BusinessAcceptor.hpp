#pragma once
#include <array>
#include <memory>
#include <functional>
#include <asio.hpp>
#include "../Log/Logger.hpp"
#include "../Network/Socket.hpp"
#include "../Network/Request.hpp"
#include "ThreadPool.hpp"
#include "Responder.hpp"
#include "ClientService.hpp"
#include "AdminService.hpp"
#include "../Network/Util.hpp"
#include "AccountManager.hpp"

namespace Server
{
	using Log::Logger;
	using Network::LoginRequest;
	using ::std::array;
	using ::std::exception;
	using ::std::function;
	using ::std::shared_ptr;
	using namespace Json;

	int i = 0;
	class BusinessAcceptor
	{
	private:
		bool _startAccept = false;
		ThreadPool* _threadPool = nullptr;
		AccountManager* _accountManager = nullptr;
		Responder _responder;
		function<ClientService(shared_ptr<Socket>, Responder*)> _clientServiceFactory;
		function<AdminService(shared_ptr<Socket>, Responder*)> _adminServiceFactory;
		Logger _logger;

	public:
		BusinessAcceptor(Responder responder, Logger logger)
			: _responder(move(responder)), _logger(move(logger))
		{
		}

		void SetThreadPool(ThreadPool* threadPool)
		{
			_threadPool = threadPool;
		}

		void SetAccountManager(AccountManager* accountManager)
		{
			_accountManager = accountManager;
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

#define ACCESS_LOG_FORMAT STRING("%h %u %i %r %a %s")
			// h: client ip
			// u: username:
			// i: request id
			// r: request function 这里可以放请求的功能: -
			// a: argument
			// s: request result status: 这里可以放请求结果的状态，可以自定义一些状态码
			// timestamp 默认  %t

			auto subLogger = _logger.GenerateSubLogger(ACCESS_LOG_FORMAT);
#undef ACCESS_LOG_FORMAT

			// async_accept 等待的时候被取消
			if (error)
			{
				subLogger.Error("Wrong connect: " + error.message());
				return;
			}
			
			auto j = i++;
			printf("client connect %d\n", j);
			auto addr = peer.Address();
			auto connectLogger = subLogger.BornNewWith(addr);
			
			_threadPool->Execute([this, p = make_shared<Socket>(move(peer)), conLogger = move(connectLogger)] () mutable
			{
				printf("start communicate\n");
				CommunicateWith(move(p), move(conLogger));
			});
			printf("client accept %d\n", j);
		}

	private:
		// peer 是连接中对端的意思
		template <typename ConnectLogger>
		void CommunicateWith(shared_ptr<Socket> peer, ConnectLogger connectLogger)
		{
			auto send = [&](string const& message)
			{
				_responder.RespondTo(peer, message);
			};

			auto greet = Receive<string, false, ByteProcessWay::Raw>(peer.get());
			if (greet == "hello server")
			{
				send("server ok");
			}
			else
			{
				send("wrong greeting words: " + greet);
				return;
			}

			auto loginInfo = Receive<LoginRequest::Content, false, ByteProcessWay::ParseThenDeserial>(peer.get());
			printf("receive account\n");
			if (_accountManager->IsRegistered(loginInfo.Username, loginInfo.Password))
			{
				auto userLogger = connectLogger.BornNewWith(loginInfo.Username);

				if (_accountManager->IsAdmin(loginInfo.Username, loginInfo.Password))
				{
					printf("admin login\n");
					send("server ok. welcome admin.");
					auto s = _adminServiceFactory(move(peer), &_responder);
					return s.Run(move(userLogger));
				}
				else
				{
					send("server ok. hello client.");
					auto s = _clientServiceFactory(move(peer), &_responder);
					return s.Run(move(userLogger));
				}
			}

			send("Account not registered. please contact admin to register.");
		}
	};
}

#pragma once
#include <filesystem>
#include "../Log/Logger.hpp"
#include "ThreadPool.hpp"
#include "NetworkAcceptor.hpp"
#include "IOContext.hpp"
#include "BusinessAcceptor.hpp"
#include "Responder.hpp"
#include "FuncLibWorker.hpp"

namespace Server
{
	using ::asio::io_context;
	using ::asio::ip::tcp;
	using ::std::move;
	namespace fs = ::std::filesystem;

	class Server
	{
	private:
		ThreadPool _threadPool;
		NetworkAcceptor _netAcceptor;
		BusinessAcceptor _businessAcceptor;
		FuncLibWorker _funcLibWorker;

	public:
		Server(ThreadPool threadPool, NetworkAcceptor netAcceptor, BusinessAcceptor businessAcceptor, FuncLibWorker funcLibWorker)
			: _threadPool(move(threadPool)), _netAcceptor(move(netAcceptor)),
			  _businessAcceptor(move(businessAcceptor)),
			  _funcLibWorker(move(funcLibWorker))
		{
			_businessAcceptor.SetThreadPool(&_threadPool);
			_funcLibWorker.SetThreadPool(&_threadPool);

			_businessAcceptor.SetServiceFactory(
				[&](shared_ptr<Socket> socket, Responder *responder) mutable { return ClientService(move(socket), &_funcLibWorker, responder); },
				[&](shared_ptr<Socket> socket, Responder *responder) mutable { return AdminService(move(socket), &_funcLibWorker, responder); });

			_netAcceptor.AsyncAccept(
				std::bind(&BusinessAcceptor::HandleAccept, &_businessAcceptor,
						  std::placeholders::_1, std::placeholders::_2));
		}

		void StartRunBackground()
		{
			_businessAcceptor.StartAcceptBackground();
		}
		
		static auto New(IoContext& ioContext, int port)
		{
			using FuncLib::FunctionLibrary;
			fs::path serverDir = R"(./server)";
			if ((not fs::exists(serverDir)) or (not fs::is_directory(serverDir)))
			{
				fs::create_directory(serverDir);
			}

			auto n = thread::hardware_concurrency();
			ThreadPool threadPool(n);
			Responder responder;
			auto logger = Log::MakeLogger("server.log");
			auto acceptor = BusinessAcceptor(move(responder), move(logger));
			auto funcLib = FunctionLibrary::GetFrom(serverDir);
			auto funcLibWorker = FuncLibWorker(move(funcLib));
			NetworkAcceptor netAcceptor = ioContext.GetNetworkAcceptorOf(port);
			return Server(move(threadPool), move(netAcceptor), move(acceptor), move(funcLibWorker));
		}
	};
}

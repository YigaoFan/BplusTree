#pragma once
#include <functional>
#include <filesystem>
#include <asio.hpp>
#include <string>
#include "../Logger/Logger.hpp"
#include "ThreadPool.hpp"
#include "BusinessAcceptor.hpp"
#include "Responder.hpp"
#include "FuncLibWorker.hpp"

namespace Server
{
	using ::asio::io_context;
	using ::asio::ip::tcp;
	using ::std::move;
	namespace fs = ::std::filesystem;

	template <typename Logger>
	class Server
	{
	private:
		ThreadPool _threadPool;
		tcp::acceptor _netAcceptor;
		BusinessAcceptor _businessAcceptor;
		FuncLibWorker _funcLibWorker;
		Logger _logger;

	public:
		Server(ThreadPool threadPool, tcp::acceptor netAcceptor, BusinessAcceptor businessAcceptor, FuncLibWorker funcLibWorker, Logger logger)
			: _threadPool(move(threadPool)), _netAcceptor(move(netAcceptor)),
			  _businessAcceptor(move(businessAcceptor)),
			  _funcLibWorker(move(funcLibWorker)), _logger(move(logger))
		{
			_businessAcceptor.SetThreadPool(&_threadPool);
			_funcLibWorker.SetThreadPool(&_threadPool);

			_businessAcceptor.SetServiceFactory(
				[&](shared_ptr<Socket> socket, Responder *responder) mutable { return ClientService(move(socket), &_funcLibWorker, responder); },
				[&](shared_ptr<Socket> socket, Responder *responder) mutable { return AdminService(move(socket), &_funcLibWorker, responder); });

			_netAcceptor.async_accept(
				std::bind(&BusinessAcceptor::HandleAccept, &_businessAcceptor,
						  std::placeholders::_1, std::placeholders::_2));
		}

		void Init()
		{
			// for future init
		}

		void StartRunBackground()
		{
			_businessAcceptor.StartAcceptBackground();
		}

		// terminate run? 有这个需求吗？
	};

#define LOG_FORMAT STRING("%h %l %u %t %r %s %b %i %i")
	auto New(io_context& ioContext, int port)
	{
		fs::path serverDir = R"(./server)";
		if ((not fs::exists(serverDir)) or (not fs::is_directory(serverDir)))
		{
			fs::create_directory(serverDir);
		}

		auto n = thread::hardware_concurrency();
		ThreadPool threadPool(n);
		Responder responder;
		auto acceptor = BusinessAcceptor(move(responder));
		using FuncLib::FunctionLibrary;
		auto funcLib = FunctionLibrary::GetFrom(serverDir);
		auto funcLibWorker = FuncLibWorker(move(funcLib));
		auto logger = Log::MakeLoggerWith(LOG_FORMAT);
		tcp::acceptor netAcceptor(ioContext, tcp::endpoint(tcp::v4(), port));
		return Server(move(threadPool), move(netAcceptor), move(acceptor), move(funcLibWorker), move(logger));
	}
#undef LOG_FORMAT
}

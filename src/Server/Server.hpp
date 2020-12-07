#pragma once
#include <functional>
#include <asio.hpp>
#include "../Logger/Logger.hpp"
#include "ThreadPool.hpp"
#include "BusinessAcceptor.hpp"
#include "Responder.hpp"
#include "InvokeWorker.hpp"

namespace Server
{
	using ::asio::io_context;
	using ::asio::ip::tcp;
	using ::std::move;

	template <typename Logger>
	class Server
	{
	private:
		tcp::acceptor _netAcceptor;
		BusinessAcceptor _businessAcceptor;
		InvokeWorker _invokeWorker;
		Logger _logger;

	public:

		Server(tcp::acceptor netAcceptor, BusinessAcceptor businessAcceptor, InvokeWorker invokeWorker, Logger logger)
			: _netAcceptor(move(netAcceptor)), _businessAcceptor(move(businessAcceptor)),
			  _invokeWorker(move(invokeWorker)), _logger(move(logger))
		{
			_businessAcceptor.SetServiceFactory(
				[&](Socket socket, Responder *responder) mutable { return ClientService(move(socket), &_invokeWorker, responder); },
				[&](Socket socket, Responder *responder) mutable { return AdminService(move(socket), &_invokeWorker, responder); });

			_netAcceptor.async_accept(
				std::bind(&BusinessAcceptor::HandleAccept, &_businessAcceptor,
						  std::placeholders::_1, std::placeholders::_2));
		}

		void Init()
		{
			// for future init
		}

		void Start()
		{
			_invokeWorker.StartRunBackground();
			_businessAcceptor.StartAcceptBackground();
		}
	};

#define LOG_FORMAT STRING("%h %l %u %t %r %s %b %i %i")
	auto New(io_context& ioContext, int port)
	{
		ThreadPool threadPool;
		Responder responder;
		auto acceptor = BusinessAcceptor(move(threadPool), move(responder));
		auto invokeWorker = InvokeWorker();
		auto logger = Log::MakeLoggerWith(LOG_FORMAT);
		tcp::acceptor netAcceptor(ioContext, tcp::endpoint(tcp::v4(), port));
		return Server(move(netAcceptor), move(acceptor), move(invokeWorker), move(logger));
	}
#undef LOG_FORMAT
}

#pragma once
#include "../Logger/Logger.hpp"
#include "ThreadPool.hpp"
#include "Acceptor.hpp"
#include "Responder.hpp"
#include "InvokeWorker.hpp"

namespace Server
{
	using ::std::move;

#define LOG_FORMAT STRING("%h %l %u %t %r %s %b %i %i")
	class Server
	{
	private:
		decltype(Log::MakeLoggerWith(LOG_FORMAT)) _logger;
		InvokeWorker _invokeWorker;
		Acceptor _acceptor;
	public:
		static Server New()
		{
			ThreadPool threadPool;
			Responder responder;
			auto acceptor = Acceptor(move(threadPool), move(responder));
			auto invokeWorker = InvokeWorker();
			auto logger = Log::MakeLoggerWith(LOG_FORMAT);
			return Server(move(acceptor), move(invokeWorker), move(logger));
		}

		Server(Acceptor acceptor, InvokeWorker invokeWorker, decltype(_logger) logger)
			: _acceptor(move(acceptor)), _invokeWorker(move(invokeWorker)), _logger(move(logger))
		{
			_acceptor.SetServiceFactory(
				[&](Socket socket, Responder *responder) { return ClientService(socket, &_invokeWorker, responder); },
				[&](Socket socket, Responder *responder) { return AdminService(socket, &_invokeWorker, responder);  });
		}

		void Init()
		{

		}

		void Start()
		{
			_invokeWorker.StartRunBackground();
			_acceptor.StartAcceptBackground();
		}
	};
#undef LOG_FORMAT
}

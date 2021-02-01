#pragma once
#include "NetworkAcceptor.hpp"

#ifdef MOCK_NET
#include <vector>

namespace Network
{
	using ::std::vector;

	class IoContext
	{
	private:
		using SendMessages = vector<string>;
		using Handler = typename NetworkAcceptor::Handler;
		vector<Handler> _connectHandlers;
		SendMessages _sendMessages;

	public:
		void SetSendMessages(SendMessages sendMessages)
		{
			_sendMessages = move(sendMessages);
		}
		
		NetworkAcceptor GetNetworkAcceptorOf(int port)
		{
			auto handlerRegister = [this](Handler handler)
			{
				_connectHandlers.push_back(move(handler));
			};
			auto acceptor = NetworkAcceptor(move(handlerRegister));
			return acceptor;
		}

		void Run()
		{
			int port = 8888;
			for (auto& h : _connectHandlers)
			{
				h({}, Socket("localhost", port, _sendMessages));
			}
		}
	};
}

#else
#include <asio.hpp>

namespace Network
{
	using asio::io_context;

	class IoContext
	{
	private:
		io_context _context;

	public:
		IoContext() { }

		NetworkAcceptor GetNetworkAcceptorOf(int port)
		{
			NetworkAcceptor acceptor(_context, tcp::endpoint(tcp::v4(), port));
			return acceptor;
		}

		Socket GetConnectedSocketTo(string const& hostname, int port)
		{
			using asio::ip::tcp;
			tcp::endpoint p(asio::ip::address::from_string(hostname), port);
			tcp::socket s(_context);
			s.connect(p);
			return move(s);
		}

		void Run()
		{
			_context.run();
		}
	};
}
#endif

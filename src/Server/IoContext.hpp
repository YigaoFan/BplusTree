#pragma once
#include "NetworkAcceptor.hpp"

#ifdef MOCK_NET

namespace Server
{
	class IoContext
	{
	private:
	public:
		IoContext();

		NetworkAcceptor GetNetworkAcceptorOf(int port)
		{
			auto acceptor = NetworkAcceptor("localhost", port);
			return acceptor;
		}

		void Run()
		{
			// 会为每个 Socket 准备一组发送的请求
			string requests[] =
			{
				"Hello Server",
				// ...
			};
		}
	};
}

#else
#include <asio.hpp>

namespace Server
{
	using ::asio::io_context;
	using ::asio::ip::tcp;

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

		void Run()
		{
			_context.run();
		}
	};
}
#endif

#pragma once
#include "NetworkAcceptor.hpp"

namespace Server
{
	using ::asio::ip::tcp;

	class IOContext
	{
	private:
		/* data */
	public:
		IOContext(/* args */);

		NetworkAcceptor GetNetworkAcceptorOf(int port)
		{

		}

		void Run()
		{

		}
	};
}

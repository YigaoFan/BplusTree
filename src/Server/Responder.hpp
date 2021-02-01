#pragma once
#include <string>
#include <memory>
#include "../Network/Socket.hpp"

namespace Server
{
	using Network::Socket;
	using ::std::shared_ptr;
	using ::std::string;

	class Responder
	{
	private:
		/* data */
	public:
		Responder(/* args */)
		{

		}

		void RespondTo(shared_ptr<Socket> peer, string const& message)
		{
			peer->Send(message);
		}
	};
}

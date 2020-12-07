#pragma once
#include <string>
#include "BasicType.hpp"

namespace Server
{
	using ::std::string;

	class Responder
	{
	private:
		/* data */
	public:
		Responder(/* args */)
		{

		}

		void RespondTo(Socket& peer, string const& message)
		{
			peer.send(asio::buffer(message));
		}
	};
}

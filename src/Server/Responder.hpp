#pragma once
#include <string>
#include <memory>
#include "BasicType.hpp"

namespace Server
{
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
			peer->send(asio::buffer(message));
		}
	};
}

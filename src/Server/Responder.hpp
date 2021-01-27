#pragma once
#include <string>
#include <memory>

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
			peer->Send(message);
		}
	};
}

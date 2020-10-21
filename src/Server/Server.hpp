#include "RequestListener.hpp"
#include "RequestResponder.hpp"
#include "RequestHandler.hpp"
#include "../Logger/Logger.hpp"

namespace Server
{
	using Log::Logger;

	class Server
	{
	private:
		Logger _logger;
		// FunctionLibrary
		RequestListener _listener;
		RequestResponder _responder;
		RequestHandler _handler;// can be multiple
	public:
		Server();
		void Init();
		void Run();
		~Server();
	};
	
	Server::Server()
	{
	}
	
	Server::~Server()
	{
	}
		
}

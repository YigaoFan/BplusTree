#pragma once
#include <functional>
#include <asio.hpp>
#include "Socket.hpp"

namespace Server
{
	using ::std::function;

	class NetworkAcceptor
	{
	private:
		function<void(asio::error_code const&, Socket)> _callback;
		
	public:
		NetworkAcceptor(/* args */);

		NetworkAcceptor(NetworkAcceptor&& that) noexcept = default;
		NetworkAcceptor(NetworkAcceptor const& that) = delete;

		void AsyncAccept(auto callback)
		{
			_callback = move(callback);
		}
	};	
}

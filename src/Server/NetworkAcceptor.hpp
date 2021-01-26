#pragma once
#include "Socket.hpp"

#ifdef MOCK_NET
#include <string>
#include <functional>

namespace Server
{
	using ::std::function;
	using ::std::string;

	class NetworkAcceptor
	{
	private:
		function<void(asio::error_code const&, Socket)> _callback;
		
	public:
		NetworkAcceptor(string ip, int port)
		{

		}

		NetworkAcceptor(NetworkAcceptor&& that) noexcept = default;
		NetworkAcceptor(NetworkAcceptor const& that) = delete;

		void AsyncAccept(auto callback)
		{
			_callback = move(callback);
		}
	};
}
#else
#include <asio.hpp>

namespace Server
{
	using ::asio::ip::tcp;
	using ::std::forward;

	class NetworkAcceptor
	{
	private:
		tcp::acceptor _netAcceptor;

	public:
		template <typename... Args>
		NetworkAcceptor(Args&&... args) : _netAcceptor(forward<Args>(args)...)
		{ }

		void AsyncAccept(auto callback)
		{
			_netAcceptor.async_accept(move(callback));
		}
	};
}
#endif

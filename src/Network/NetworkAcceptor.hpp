#pragma once
#include "Socket.hpp"

#ifdef MOCK_NET
#include <string>
#include <functional>
#include <asio.hpp>

namespace Network
{
	using ::std::function;
	using ::std::move;
	using ::std::string;

	class NetworkAcceptor
	{
	public:
		using Handler = function<void(asio::error_code const &, Socket)>;

	private:
		function<void(Handler)> _handlerRegister;
		
	public:
		NetworkAcceptor(function<void(Handler)> handlerRegister)
			: _handlerRegister(move(handlerRegister))
		{ }

		NetworkAcceptor(NetworkAcceptor&& that) noexcept = default;
		NetworkAcceptor(NetworkAcceptor const& that) = delete;

		void AsyncAccept(auto callback)
		{
			_handlerRegister(move(callback));
		}
	};
}
#else
#include <asio.hpp>

namespace Network
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

		void Close()
		{
			if (_netAcceptor.is_open())
			{
				_netAcceptor.close();
			}
		}
	};
}
#endif

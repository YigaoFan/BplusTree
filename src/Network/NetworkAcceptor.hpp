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
		function<void()> _close;

	public:
		NetworkAcceptor(function<void(Handler)> handlerRegister, function<void()> close)
			: _handlerRegister(move(handlerRegister)), _close(move(close))
		{ }

		NetworkAcceptor(NetworkAcceptor&& that) noexcept = default;
		NetworkAcceptor(NetworkAcceptor const& that) = delete;

		void AsyncAccept(auto callback)
		{
			_handlerRegister(move(callback));
		}

		void Close()
		{
			_close();
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

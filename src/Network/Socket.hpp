#pragma once
#include <string>

#ifdef MOCK_NET
#include <vector>
#include <exception>

namespace Network
{
	using ::std::string;
	using ::std::vector;
	using ::std::move;

	class Socket
	{
	private:
		string _ip;
		int _port;
		vector<string> _receiveMessages;
		vector<string> _sendMessages;
		int _currentSendIndex = 0;

	public:
		Socket(string ip, int port, vector<string> sendMessages)
			: _ip(move(ip)), _port(port), _sendMessages(move(sendMessages))
		{ }
		Socket(Socket&& that) noexcept = default;
		Socket(Socket const& that) = delete;

		/// "{ip}:{port}"
		string Address() const
		{
			auto addr = _ip;
			return addr.append(":" + std::to_string(_port));
		}

		vector<string> const& ReceivedMessage() const
		{
			return _receiveMessages;
		}

		void Send(string const& message)
		{
			// printf("client receive: %s\n", message.c_str());
			_receiveMessages.push_back(message);
		}

		string Receive()
		{
			if (_currentSendIndex < _sendMessages.size())
			{
				return _sendMessages[_currentSendIndex++];
			}
			throw std::runtime_error("end of file");
		}

		// 要不要析构函数里设置当有效的 Socket 析构时打一个下线 log 呢 TODO
	};
}	

#else
#include <asio.hpp>
#include <array>

namespace Network
{
	using asio::ip::tcp;
	using ::std::array;
	using ::std::move;
	using ::std::string;

	// async_accept 竟然可以匹配上这个类型
	class Socket
	{
	private:
		tcp::socket _peer;
		array<char, 512> _buff;

	public:
		Socket(tcp::socket&& peer) noexcept : _peer(move(peer))
		{ }

		/// "{ip}:{port}"
		string Address() const
		{
			auto p = _peer.remote_endpoint().port();
			return _peer.remote_endpoint().address().to_string().append(":" + std::to_string(p));
		}

		void Send(string const& message)
		{
			string size;
			size.push_back(static_cast<int>(message.size()));
			_peer.send(asio::buffer(size));
			_peer.send(asio::buffer(message));
		}

		string Receive()
		{
			asio::streambuf buf0;
			asio::read(_peer, buf0, asio::transfer_exactly(1));
			auto n = buf0.sgetc();
			asio::streambuf buf1;
			asio::read(_peer, buf1, asio::transfer_exactly(n));
			return string(asio::buffers_begin(buf1.data()), asio::buffers_end(buf1.data()));
		}
	};
}
#endif

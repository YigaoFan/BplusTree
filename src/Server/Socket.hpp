#pragma once
#include <array>
#include <string>
#include <vector>
#include <asio.hpp>

namespace Server
{
	using ::std::string;
	using ::std::vector;
	using ::std::move;

#ifdef MOCK_NET
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

		void Send(string const& message)
		{
			_receiveMessages.push_back(message);
		}

		string Receive()
		{
			return _sendMessages[_currentSendIndex++];
		}

		// 要不要析构函数里设置当有效的 Socket 析构时打一个下线 log 呢 TODO
	};	

#else
	using asio::ip::tcp;
	using ::std::array;

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
			_peer.send(asio::buffer(message));
		}

		string Receive()
		{
			auto n = _peer.receive(asio::buffer(_buff));
			return string(_buff.data(), n);
		}
	};
#endif
}

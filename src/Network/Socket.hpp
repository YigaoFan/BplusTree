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
#include <vector>

namespace Network
{
	using asio::ip::tcp;
	using ::std::array;
	using ::std::move;
	using ::std::string;
	using ::std::vector;

	// async_accept 竟然可以匹配上这个类型
	class Socket
	{
	private:
		// 下面这个值需要根据网络情况来调整大小，复杂的解决方法就是建立丢包重发机制
		static constexpr int PackageSize = 1 * 1024;
		tcp::socket _peer;
		array<char, PackageSize> _buff;

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
			// 其实这里用了网络牵涉到大端和小端
			auto n = static_cast<int>(message.size());
			string size(reinterpret_cast<char const*>(&n), sizeof(int));
			_peer.send(asio::buffer(size));
			auto remain = n;
			auto i = 0;
			do
			{
				auto currentWriteSize = remain > PackageSize ? PackageSize : remain;
				_peer.send(asio::buffer(message.substr(i, currentWriteSize)));
				remain -= currentWriteSize;
				i += currentWriteSize;
			} while (remain > 0);
		}

		string Receive()
		{
			asio::streambuf buf0;
			asio::read(_peer, buf0, asio::transfer_exactly(sizeof(int)));
			auto sizeBytes = string(asio::buffers_begin(buf0.data()), asio::buffers_end(buf0.data()));
			auto n = *reinterpret_cast<int const*>(sizeBytes.data());
			// printf("plan to receive %d\n", n);
			string receive;
			receive.reserve(n);

			auto remain = n;
			do
			{
				auto currentReadSize = remain > PackageSize ? PackageSize : remain;
				asio::read(_peer, asio::buffer(_buff, PackageSize), asio::transfer_exactly(currentReadSize));
				receive.append(_buff.data(), currentReadSize);
				remain -= currentReadSize;
			} while (remain > 0);
			// printf("received %d\n", n);

			return receive;
		}
	};
}
#endif

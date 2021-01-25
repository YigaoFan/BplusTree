#pragma once
#include <string>

namespace Server
{
	using ::std::string;

	class Socket
	{
	private:
		/* data */
	public:
		Socket() { }
		Socket(Socket&& that) noexcept = default;
		Socket(Socket const& that) = delete;

		string IPAddress() const
		{

		}

		void Send(string const& message)
		{

		}

		string Receive()
		{

		}

		// 要不要析构函数里设置当有效的 Socket 析构时打一个下线 log 呢
	};	
}

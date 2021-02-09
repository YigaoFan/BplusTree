#pragma once
#include <array>
#include <vector>
#include <string>
#include <asio.hpp>
#include <string_view>
#include "StringMatcher.hpp"
#include "../Server/CmdFunction.hpp"
#include "../Network/Socket.hpp"
#include "../TestFrame/Util.hpp"
using namespace Test;

namespace Cmd
{
	using Network::Socket;
	using Server::GetCmdsName;
	using Server::ProcessCmd;
	using ::std::array;
	using ::std::move;
	using ::std::string;
	using ::std::string_view;
	using ::std::vector;

	// 运行命令，出异常了不要让程序停止，其他情况异常要抛出去
	class Cmder
	{
	private:
		StringMatcher _cmdMatcher;
		Socket _socket;

	public:
		static Cmder NewFrom(Socket connectedSocket)
		{
			vector<string> cmds = GetCmdsName();
			cmds.push_back("exit"); // 这里稍有点不好，或许可以提供给外部注册进来
			return Cmder(StringMatcher(move(cmds)), move(connectedSocket));
		}

		Cmder(StringMatcher cmdMatcher, Socket connectedSocket)
			: _cmdMatcher(move(cmdMatcher)), _socket(move(connectedSocket))
		{ }
		
		/// return maybe multiple lines
		vector<string> Run(string cmd)
		{
			// 这里可以从哪获取一点被调用接口的信息然后进行提示和检查
			// 使用一些代码去生成
			auto [requests, responseProcessor] = ProcessCmd(cmd);
			for (auto& r : requests)
			{
				_socket.Send(r);
			}
			auto id = _socket.Receive();
			auto response = _socket.Receive();
			return responseProcessor(response);
		}

		/// return is multiple options
		vector<string> Complete(string_view partKeyword)
		{
			return _cmdMatcher.Match(partKeyword);
		}
	};	
}

#pragma once
#include <array>
#include <vector>
#include <string>
#include <asio.hpp>
#include <string_view>
#include "StringMatcher.hpp"
#include "../Server/CmdMetadata.hpp"

namespace Cmd
{
	using Server::GenerateSendBytes;
	using Server::GetCmdsName;
	using ::std::array;
	using ::std::move;
	using ::std::string;
	using ::std::string_view;
	using ::std::vector;
	using Socket = asio::ip::tcp::socket;

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
			return Cmder(StringMatcher(move(cmds)), move(connectedSocket));
		}

		Cmder(StringMatcher cmdMatcher, Socket connectedSocket)
			: _cmdMatcher(move(cmdMatcher)), _socket(move(connectedSocket))
		{ }
		
		/// return maybe multiple lines
		string Run(string cmd)
		{
			// 这里可以从哪获取一点被调用接口的信息然后进行提示和检查
			// 使用一些代码去生成
			string bytes = GenerateSendBytes(cmd);
			_socket.send(asio::buffer(bytes));

			array<char, 1024> buff;
			auto n = _socket.receive(asio::buffer(buff));//如何设置 block 的时限呢？
			auto output = string(buff.data(), n);
			return output;
		}

		/// return is multiple options
		vector<string> Complete(string_view partKeyword)
		{
			return _cmdMatcher.Match(partKeyword);
		}
	};	
}

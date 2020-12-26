#pragma once
#include <array>
#include <vector>
#include <string>
#include <asio.hpp>
#include "StringMatcher.hpp"
#include "../Server/CmdMetadata.hpp"

namespace Cmd
{
	using Server::GetCmdsName;
	using ::std::array;
	using ::std::move;
	using ::std::string;
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

		/// return is multiple lines
		vector<string> Run(string cmd)
		{
			// 这里可以从哪获取一点被调用接口的信息然后进行提示和检查
			// 使用一些代码去生成
			// process and dispatch cmd to different type struct then serialize to invokeInfo
			string invokeInfo;
			_socket.send(asio::buffer(invokeInfo));

			array<char, 1024> buff;
			auto n = _socket.receive(asio::buffer(buff));//如何设置 block 的时限呢？
			auto output = string_view(buff.data(), n);
			return { string(output) };
		}

		/// return is multiple options
		vector<string> Complete(string partKeyword)
		{
			return _cmdMatcher.Match(partKeyword);
		}
	};	
}

#pragma once
#include <array>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include "StringMatcher.hpp"
#include <asio.hpp>

namespace Cmd
{
	using ::std::array;
	using ::std::ifstream;
	using ::std::move;
	using ::std::string;
	using ::std::vector;
	using ::std::filesystem::path;
	using Socket = asio::ip::tcp::socket;

	class Cmder
	{
	private:
		StringMatcher _cmdMatcher;
		Socket _socket;

	public:
		static Cmder NewFrom(path const& cmdFilename, Socket connectedSocket)
		{
			using ::std::find_if_not;
			using ::std::isspace;

			ifstream fs(cmdFilename, ifstream::in);
			vector<string> cmds;

			string line;
			while (std::getline(fs, line))
			{
				// # 为注释符号
				if (not line.starts_with('#'))
				{
					// trim from start and end
					line.erase(line.begin(), find_if_not(line.begin(), line.end(), [](char c)
					{
						return isspace(c);
					}));
					line.erase(find_if_not(line.rbegin(), line.rend(), [](char c)
					{
						return isspace(c);
					}).base(), line.end());

					cmds.push_back(move(line));
				}
			}

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

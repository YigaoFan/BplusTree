#pragma once
#include <array>
#include <vector>
#include <string>
#include <string_view>
#include "BasicType.hpp"
#include "Request.hpp"

namespace Server
{
	using ::std::array;
	using ::std::move;
	using ::std::string;
	using ::std::string_view;
	using ::std::vector;

	// 这里的代码应该可以简化 TODO
#define nameof(VAR) #VAR
	/// Get admin command names
	constexpr vector<string> GetCmdsName()
	{
		return
		{
			nameof(AddFunc),
			nameof(RemoveFunc),
			nameof(SearchFunc),
			nameof(ModifyFunc),
		};
	}
#undef nameof

	string HandleAddFunc(Socket* connectedSocket, string cmd)
	{
		// use stringstream divide cmd?
		// parse package to vector<string>
		vector<string> package;
		// obtain func def
		string funcsDef;
		// get summary
		string summary;

		AddFuncRequest::Content content
		{
			move(package),
			move(funcsDef),
			move(summary)
		};

		auto s = Json::JsonConverter::Serialize(content).ToString();
		connectedSocket->send(asio::buffer(s));
		array<char, 1024> buff;
		auto n = _socket.receive(asio::buffer(buff)); //如何设置 block 的时限呢？
		auto output = string_view(buff.data(), n);
		return output;
	}

	string HandleRemoveFunc(Socket* connectedSocket, string cmd)
	{
		auto socket = connectedSocket;

		// 下面这些内容是不是也可以样板化
		string returnType;
		string funcName;
		vector<string> argTypes;
		vector<string> package;

		FuncType func(move(returnType), move(funcName), move(argTypes), move(package));

		RemoveFuncRequest::Content content
		{
			move(func),
		};

		auto s = Json::JsonConverter::Serialize(content).ToString();
		socket->send(asio::buffer(s));
		array<char, 1024> buff;
		auto n = _socket.receive(asio::buffer(buff)); //如何设置 block 的时限呢？
		auto output = string_view(buff.data(), n);
		return output;
	}

	vector<string> HandleSearchFunc(Socket* connectedSocket, string cmd)
	{
		auto socket = connectedSocket;

		string keyword;

		SearchFuncRequest::Content content
		{
			move(keyword),
		};

		auto s = Json::JsonConverter::Serialize(content).ToString();
		socket->send(asio::buffer(s));
		//如何设置 block 的时限呢？
		// socket->set_option(asio::detail::socket_option::integer<SOL_SOCKET, SO_RCVTIMEO>{200});

		array<char, 1024> buff;
		auto n = _socket.receive(asio::buffer(buff));
		auto output = string_view(buff.data(), n);
		// 这里要解析下，因为 json
		return output;
	}
}

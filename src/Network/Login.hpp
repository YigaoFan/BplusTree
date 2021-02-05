#pragma once
#include <string>
#include "../Basic/Exception.hpp"
#include "Socket.hpp"
#include "Request.hpp"
#include "../Json/JsonConverter/JsonConverter.hpp"

namespace Network
{
	using ::Basic::InvalidOperationException;
	using ::std::string;

	void Login(Socket* peer, bool isAdmin, LoginRequest::Content const& loginInfo)
	{
		peer->Send("hello server");
		auto r1 = peer->Receive();
		if (r1 != "server ok")
		{
			throw InvalidOperationException("server response greet error: " + r1);
		}

		peer->Send(Json::JsonConverter::Serialize(loginInfo).ToString());
		auto r2 = peer->Receive();
		auto des = isAdmin ? "server ok. welcome admin." : "server ok. hello client.";
		if (r2 != des)
		{
			throw InvalidOperationException("server response login error: " + r2);
		}
	}
}

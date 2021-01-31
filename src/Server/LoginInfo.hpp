#pragma once
#include <string>
#include "../Json/JsonConverter/JsonConverter.hpp"

namespace Server
{
	using ::std::string;

	struct LoginInfo
	{
		string Username;
		string Password;
	};
}

namespace Json::JsonConverter
{
	using Server::LoginInfo;

	template <>
	JsonObject Serialize(LoginInfo const &account)
	{
#define nameof(VAR) #VAR
		auto [username, password] = account;
		JsonObject::_Object obj;
		obj.insert({nameof(username), Serialize(username)});
		obj.insert({nameof(password), Serialize(password)});

		return JsonObject(move(obj));
	}

	template <>
	LoginInfo Deserialize(JsonObject const &jsonObj)
	{
		auto username = Deserialize<string>(jsonObj[nameof(username)]);
		auto password = Deserialize<string>(jsonObj[nameof(password)]);

		return LoginInfo{username, password};
	}
#undef nameof
}
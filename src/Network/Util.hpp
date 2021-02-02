#pragma once
#include <string>
#include <utility>
#include <stdexcept>
#include <type_traits>
#include <string_view>
#include "../Network/Socket.hpp"
#include "../Json/Json.hpp"
#include "../Json/Parser.hpp"
#include "../Json/JsonConverter/JsonConverter.hpp"

namespace Network
{
	using Json::JsonObject;
	using Network::Socket;
	using ::std::is_same_v;
	using ::std::pair;
	using ::std::runtime_error;
	using ::std::string;
	using ::std::string_view;

	enum class ByteProcessWay
	{
		Raw,
		ParseThenDeserial,
	};

	template <typename Return, bool ReturnAdditionalRawByte, ByteProcessWay ByteProcessWay>
	auto Receive(Socket* peer)
	{
		auto input = peer->Receive();
		// 抛异常不要紧，重要的是在抛之前把异常 log 下来
		// 还有异常不一定是问题，比如对端正常的断开连接——这个会触发异常吗？
		if constexpr (ByteProcessWay == ByteProcessWay::ParseThenDeserial)
		{
			auto jsonObj = Json::Parse(input);
			auto ret = Json::JsonConverter::Deserialize<Return>(jsonObj);

			if constexpr (ReturnAdditionalRawByte)
			{
				return pair<Return, string>(move(ret), move(input));
			}
			else
			{
				return ret;
			}
		}
		else if constexpr (ByteProcessWay == ByteProcessWay::Raw)
		{
			if constexpr (ReturnAdditionalRawByte)
			{
				return pair<string, string>(string(input), move(input));
			}
			else
			{
				return input;
			}
		}
	}

	template <typename Return>
	Return HandleOperationResponse(string_view response)
	{
		JsonObject result = Json::Parse(response);

		if (result["type"].GetString() == "exception")
		{
			auto message = result["value"].GetString();
			throw runtime_error(move(message));
		}

		if constexpr (is_same_v<Return, void>)
		{
			return;
		}
		else
		{
			return Json::JsonConverter::Deserialize<Return>(result["value"]);
		}
	}
}

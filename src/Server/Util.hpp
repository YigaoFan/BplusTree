#pragma once
#include <array>
#include <string>
#include <utility>
#include <string_view>
#include "BasicType.hpp"
#include "../Json/Parser.hpp"
#include "../Json/JsonConverter/JsonConverter.hpp"

namespace Server
{
	using ::std::array;
	using ::std::exception;
	using ::std::pair;
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
		array<char, 512> buff;
		auto n = peer->receive(asio::buffer(buff));

		// if (error)
		// {
		// 	// 抛异常不要紧，重要的是在抛之前把异常 log 下来
		// 	string message;
		// 	if (error == asio::error::eof)
		// 	{
		// 		message = string("Client disconnect: " + error.message());
		// 	}
		// 	else
		// 	{
		// 		message = string("Read from client error: " + error.message());
		// 	}

		// 	logger->Error(message);
		// 	throw as; // TODO exception
		// }
		// else
		// {
			auto input = string_view(buff.data(), n);
			// try
			// {
				if constexpr (ByteProcessWay == ByteProcessWay::ParseThenDeserial)
				{
					auto jsonObj = Json::Parse(input);
					auto ret = Json::JsonConverter::Deserialize<Return>(jsonObj);

					if constexpr (ReturnAdditionalRawByte)
					{
						return pair<Return, string>(move(ret), string(input));
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
						return pair<string, string>(string(input), string(input));
					}
					else
					{
						return string(input);
					}
				}
			// }
			// catch (exception const &e)
			// {
			// 	logger->Error("Parse client content or deserialize error: ", e);
			// 	throw e;
			// }
		// }
	}
}

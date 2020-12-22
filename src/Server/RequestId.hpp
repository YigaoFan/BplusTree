#pragma once
#include <string>
#include <random>

namespace Server
{
	using ::std::string;

	string GenerateRequestId()
	{
		string src = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		std::random_device rd;
		std::mt19937 generator(rd());

		std::shuffle(src.begin(), src.end(), generator);

		return src.substr(0, 15);
	}
}

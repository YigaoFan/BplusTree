#include "Util.hpp"
#include <random>

namespace FuncLib::Compile
{
	using ::std::string;
	
	string RandomString()
	{
		string src = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		std::random_device rd;
		std::mt19937 generator(rd());

		std::shuffle(src.begin(), src.end(), generator);

		return src.substr(0, 5);
	}
}
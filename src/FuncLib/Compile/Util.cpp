#include "Util.hpp"
#include <random>
#include <fstream>

namespace FuncLib::Compile
{
	using ::std::ifstream;
	using ::std::string;

	string RandomString()
	{
		string src = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		std::random_device rd;
		std::mt19937 generator(rd());

		std::shuffle(src.begin(), src.end(), generator);

		return src.substr(0, 5);
	}

	vector<char> ReadFileBytes(char const* filename)
	{
		ifstream reader(filename, ifstream::binary);
		reader.unsetf(ifstream::skipws); // Stop eating new lines in binary mode
		reader.seekg(0, ifstream::end);
		auto size = reader.tellg();
		reader.clear();
		reader.seekg(0, ifstream::beg);

		vector<char> bytes(size);
		reader.read(reinterpret_cast<char *>(bytes.data()), size);
		return bytes;
	}
}
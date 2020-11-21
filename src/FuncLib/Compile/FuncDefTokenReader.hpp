#include <string>
#include <fstream>
#include "../Store/Generator.hpp"

namespace FuncLib::Compile
{
	using ::std::ifstream;
	using ::std::move;
	using ::std::string;
	using Store::Generator;

	class FuncDefTokenReader
	{
	private:
		char _delimiter;
		ifstream _istream;
	public:
		FuncDefTokenReader(decltype(_delimiter) delimiter, decltype(_istream) istream);
		void Delimiter(decltype(_delimiter) newDelimiter);
		Generator<string> GetTokenGenerator();
		bool AtEnd() const;
	};
}

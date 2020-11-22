#pragma once
#include <string>
#include <fstream>
#include <functional>
#include "../Store/Generator.hpp"

namespace FuncLib::Compile
{
	using ::std::ifstream;
	using ::std::move;
	using ::std::string;
	using ::std::function;
	using Store::Generator;

	class FuncDefTokenReader
	{
	private:
		function<bool(char)> _delimiterPredicate;
		ifstream _istream;
	public:
		FuncDefTokenReader(decltype(_istream) istream);
		void DelimiterPredicate(decltype(_delimiterPredicate) newDelimiter);
		Generator<string> GetTokenGenerator();
		bool AtEnd() const;
	};
}

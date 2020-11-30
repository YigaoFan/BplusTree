#pragma once
#include <string>
#include <fstream>
#include <functional>
#include "../../Btree/Generator.hpp"

namespace FuncLib::Compile
{
	using Collections::Generator;
	using ::std::function;
	using ::std::ifstream;
	using ::std::move;
	using ::std::string;

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
		void ResetReadPos();
	};
}

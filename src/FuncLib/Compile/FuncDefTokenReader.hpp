#pragma once
#include <string>
#include <memory>
#include <fstream>
#include <functional>
#include "../../Btree/Generator.hpp"

namespace FuncLib::Compile
{
	using Collections::Generator;
	using ::std::function;
	using ::std::istream;
	using ::std::move;
	using ::std::string;
	using ::std::unique_ptr;

	class FuncDefTokenReader
	{
	private:
		function<bool(char)> _delimiterPredicate;
		unique_ptr<istream> _istream;
	public:
		FuncDefTokenReader(decltype(_istream) istream);
		void DelimiterPredicate(decltype(_delimiterPredicate) newDelimiter);
		Generator<string> GetTokenGenerator();
		bool AtEnd() const;
		void ResetReadPos();
		Generator<string> GetLineCodeGenerator();
	};
}

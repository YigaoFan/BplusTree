#pragma once
#include <string>
#include <memory>
#include <fstream>
#include "../Btree/Generator.hpp"

namespace FuncLib::Compile
{
	using Collections::Generator;
	using ::std::istream;
	using ::std::move;
	using ::std::string;
	using ::std::unique_ptr;

	class FuncsDefReader
	{
	private:
		unique_ptr<istream> _istream;
	public:
		FuncsDefReader(decltype(_istream) istream);
		void ResetReadPos();
		Generator<string> GetLineCodeGenerator();
	};
}

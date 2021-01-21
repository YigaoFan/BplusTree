#include "FuncsDefReader.hpp"

namespace FuncLib::Compile
{
	using ::std::getline;

	FuncsDefReader::FuncsDefReader(decltype(_istream) istream)
		: _istream(move(istream))
	{ }

	void FuncsDefReader::ResetReadPos()
	{
		_istream->clear();
		_istream->seekg(0, istream::beg);
	}

	Generator<string> FuncsDefReader::GetLineCodeGenerator()
	{
		while (not _istream->eof())
		{
			string line;
			getline(*_istream, line);
			line.push_back('\n');
			
			co_yield move(line);
			line.clear();
		}
	}
}

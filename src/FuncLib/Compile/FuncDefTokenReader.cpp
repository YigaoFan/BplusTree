#include "FuncDefTokenReader.hpp"

namespace FuncLib::Compile
{
	using ::std::getline;

	FuncDefTokenReader::FuncDefTokenReader(decltype(_istream) istream)
		: _istream(move(istream))
	{ }

	void FuncDefTokenReader::DelimiterPredicate(decltype(_delimiterPredicate) delimiterPredicate)
	{
		_delimiterPredicate = move(delimiterPredicate);
	}

	Generator<string> FuncDefTokenReader::GetTokenGenerator()
	{
		string buffer;

		while (not _istream->eof())
		{
			auto c = _istream->get();
			if (_delimiterPredicate(c))
			{
				co_yield move(buffer);
				buffer.clear();
			}
			else
			{
				buffer.push_back(c);
			}
		}

		co_yield move(buffer); // 怎么结束 yield，像 C# 里是 yield break;
	}

	bool FuncDefTokenReader::AtEnd() const
	{
		return _istream->eof();
	}

	void FuncDefTokenReader::ResetReadPos()
	{
		_istream->clear();// 这句到底需不需要？TODO
		_istream->seekg(0, istream::beg);
	}

	Generator<string> FuncDefTokenReader::GetLineCodeGenerator()
	{
		while (not _istream->eof())
		{
			string line;
			getline(*_istream, line);
			co_yield move(line);
			line.clear();
		}
	}
}

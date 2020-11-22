#include "FuncDefTokenReader.hpp"

namespace FuncLib::Compile
{
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

		while (not _istream.eof())
		{
			auto c = _istream.get();
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
		return _istream.eof();
	}
}

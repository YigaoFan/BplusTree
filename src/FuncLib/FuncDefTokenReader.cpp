#include "FuncDefTokenReader.hpp"

namespace FuncLib
{
	FuncDefTokenReader::FuncDefTokenReader(decltype(_delimiter) delimiter, decltype(_istream) istream)
		: _delimiter(move(delimiter)), _istream(move(istream))
	{ }

	void FuncDefTokenReader::Delimiter(decltype(_delimiter) newDelimiter)
	{
		_delimiter = move(newDelimiter);
	}

	Generator<string> FuncDefTokenReader::GetTokenGenerator()
	{
		string buffer;

		while (not _istream.eof())
		{
			auto c = _istream.get();
			if (c == _delimiter)
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

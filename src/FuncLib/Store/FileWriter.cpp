#include "FileWriter.hpp"

namespace FuncLib::Store
{
	void FileWriter::Write(vector<byte> const &data)
	{
		auto p = pos_;
		pos_ += data.size();
		Write(*filename_, p, data.begin(), data.end());
	}
}

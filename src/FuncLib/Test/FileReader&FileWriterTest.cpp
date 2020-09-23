#include <string>
#include "Util.hpp"
#include "../../TestFrame/FlyTest.hpp"
#include "../Store/FileReader.hpp"
#include "../Store/FileWriter.hpp"

using namespace FuncLib::Store;
using namespace std;
using namespace FuncLib::Test;

TESTCASE("FileReader&FileWriter test")
{
	auto filename = "reader&writer";
	{
		auto writer = FileWriter(MakeFilePath(filename), 0);
		string s = "Hello World";
		writer.Write(s.c_str(), s.size());
		writer.Flush();
		ASSERT(writer.CurrentPos() == s.size());

		auto reader = FileReader(MakeFilePath("reader&writer"), 0);
		ASSERT(reader.GetLessOwnershipFile() == nullptr);

		auto bytes = reader.Read(s.size());
		ASSERT(bytes.size() == s.size());
		for (auto i = 0; i < s.size(); ++i)
		{
			ASSERT(s[i] == static_cast<char>(bytes[i]));
		}

		auto zeroBytes = reader.Read<0>();
		static_assert(zeroBytes.size() == 0);
	}// 提前让 writer 析构去 flush 到文件，以免下面 remove 之后又 flush

	remove(filename);
}

DEF_TEST_FUNC(fileReader_FileWriterTest)
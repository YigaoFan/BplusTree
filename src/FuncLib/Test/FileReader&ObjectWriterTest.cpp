#include <string>
#include "Util.hpp"
#include "../../TestFrame/FlyTest.hpp"
#include "../Store/FileReader.hpp"
#include "../Store/ObjectBytes.hpp"

using namespace FuncLib::Store;
using namespace std;
using namespace FuncLib::Test;

TESTCASE("FileReader&ObjectBytes test")
{
	auto filename = "reader&writer";
	{
		auto writer = ObjectBytes(0);// TODO pos_lable
		string s = "Hello World";
		// writer.StartCounter();
		// writer.Write(s.c_str(), s.size());
		// writer.EndCounter();
		// auto c = writer.CounterNum();
		// ASSERT(c == s.size());

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

DEF_TEST_FUNC(fileReader_ObjectWriterTest)
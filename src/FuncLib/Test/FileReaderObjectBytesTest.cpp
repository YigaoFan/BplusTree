#include <string>
#include "Util.hpp"
#include "../../TestFrame/FlyTest.hpp"
#include "../Store/FileReader.hpp"
#include "../Store/ObjectBytes.hpp"

using namespace FuncLib::Store;
using namespace std;
using namespace FuncLib::Test;

TESTCASE("FileReaderObjectBytes test")
{
	string s = "Hello World";
	constexpr auto path = "reader&writer";

	SECTION("ObjectBytes")
	{
		auto writer = ObjectBytes(0);
		writer.Add(s.c_str(), s.size());
		ASSERT(writer.Written());
		ofstream of{ path };
		writer.WriteIn(&of, 0);
	}

	SECTION("FileReader")
	{
		auto reader = FileReader::MakeReader(nullptr, MakeFilePath(path), 0);
		ASSERT(reader.GetLessOwnershipFile() == nullptr);

		auto bytes = reader.Read(s.size());
		ASSERT(bytes.size() == s.size());
		for (auto i = 0; i < s.size(); ++i)
		{
			auto c = static_cast<char>(bytes[i]);
			ASSERT(s[i] == c);
		}

		auto zeroBytes = reader.Read<0>();
		static_assert(zeroBytes.size() == 0);
		Cleaner c(path);
	}
}

DEF_TEST_FUNC(TestFileReaderObjectBytes)
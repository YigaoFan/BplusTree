#include <string>
#include <memory>
#include "../../TestFrame/FlyTest.hpp"
#include "../BtreeConverter.hpp"
// #include "../Store/File.hpp

using namespace FuncLib;
using namespace Collections;
using namespace std;

TESTCASE("Disk Btree converter")
{
	// almost tests here are same as BtreeTest.cpp
	using Btr = Btree<4, string, string>;
	// auto lessThan = [](string const &a, string const &b) { return a < b; };

	// Btr b(lessThan);
	// auto f = make_shared<File>("data");
	// auto p = BtreeConverter<string, string, 4>::ConvertToDisk(b, f);
	SECTION("TODO")
	{
	}
}

DEF_TEST_FUNC(diskBtreeTest)

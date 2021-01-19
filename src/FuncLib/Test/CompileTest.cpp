#include <fstream>
#include "../TestFrame/FlyTest.hpp"
#include "../Compile/CompileProcess.hpp"
#include "../Compile/ParseFunc.hpp"

using namespace FuncLib::Compile;
using namespace std;

TESTCASE("CompileTest")
{
	SECTION("ParseFunc")
	{
		size_t i = 0;

		SECTION("Balance1")
		{
			ASSERT(CheckBracesBalance("{}", i).first);
		}

		SECTION("Balance2")
		{
			ASSERT(CheckBracesBalance("{ { {} } {}}", i).first);
		}

		SECTION("Balance3")
		{
			ASSERT(CheckBracesBalance("{ {  } } ", i).first);
		}

		SECTION("Balance4")
		{
			ASSERT(not CheckBracesBalance("{ { {} {}}", i).first);
		}

		// } 多的情况也会被判为 balance
		SECTION("Balance5")
		{
			ASSERT(not CheckBracesBalance("{ {  {}}", i).first);
		}

		SECTION("Balance5")
		{
			ASSERT(CheckBracesBalance("{ {  {}}}}", i).first);
		}

		SECTION("Parse func")
		{
			auto funcs = ParseFunc("void Func(int a, int b) { }");
		}
		// auto filename = "FuncDef.cpp";
		// ifstream fs(filename, ifstream::in | ifstream::binary);
		// auto reader = FuncDefTokenReader(make_unique<ifstream>(move(fs)));
		// Compile(move(reader));
	}
}

DEF_TEST_FUNC(TestCompile)
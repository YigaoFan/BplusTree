#include <fstream>
#include "../../TestFrame/FlyTest.hpp"
#include "../Compile/CompileProcess.hpp"

using namespace FuncLib::Compile;
using namespace std;

TESTCASE("CompileTest")
{
	SECTION("ParseFunc")
	{
		auto filename = "FuncDef.cpp";
		ifstream fs(filename, ifstream::in | ifstream::binary);
		auto reader = FuncDefTokenReader(make_unique<ifstream>(move(fs)));
		Compile(&reader);
	}
}

DEF_TEST_FUNC(compileTest)
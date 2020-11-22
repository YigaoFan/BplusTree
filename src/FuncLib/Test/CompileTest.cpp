#include "../../TestFrame/FlyTest.hpp"
#include "../Compile/CompileProcess.hpp"

using namespace FuncLib::Compile;

TESTCASE("CompileTest")
{
	SECTION("ParseFunc")
	{
		auto filename = "FuncDef.cpp";
		ifstream fs(filename, ifstream::in | ifstream::binary);

		auto reader = FuncDefTokenReader(move(fs));
		Compile(&reader);
	}
}

DEF_TEST_FUNC(compileTest)
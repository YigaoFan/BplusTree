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
			SECTION("Simple")
			{
				auto funcs = ParseFunc("void Func(int a, int b) { }");
			}

			SECTION("Complex1")
			{
				auto code = "#include <string>\n"
							"using namespace std;\n"
							"void Func(int a, double b) { }";
				auto fs = ParseFunc(code);

				ASSERT(fs.size() == 1);
				auto f = fs[0];
				auto t = std::get<0>(f);
				ASSERT(t.FuncName == "Func");
				ASSERT(t.ArgTypes.size() == 2);
				ASSERT(t.ReturnType == "void");
			}

			SECTION("Complex2")
			{
				auto code = "#include <string>\n"
							"using namespace std;\n"
							"void Func1 (int a, double b) { }\n"
							"int Func(int a, double b) { }";
				auto fs = ParseFunc(code);

				ASSERT(fs.size() == 2);

				{
					auto f = fs[0];
					auto t = std::get<0>(f);
					ASSERT(t.FuncName == "Func1");
					ASSERT(t.ArgTypes.size() == 2);
					ASSERT(t.ReturnType == "void");
				}

				{
					auto f = fs[1];
					auto t = std::get<0>(f);
					ASSERT(t.FuncName == "Func");
					ASSERT(t.ArgTypes.size() == 2);
					ASSERT(t.ReturnType == "int");
				}
			}
		}
		// auto filename = "FuncDef.cpp";
		// ifstream fs(filename, ifstream::in | ifstream::binary);
		// auto reader = FuncDefTokenReader(make_unique<ifstream>(move(fs)));
		// Compile(move(reader));
	}
}

DEF_TEST_FUNC(TestCompile)
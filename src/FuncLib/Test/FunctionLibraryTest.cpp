#include "../TestFrame/FlyTest.hpp"
#include <sstream>
#define private public
#include "../FunctionLibrary.hpp"

using namespace FuncLib;
using namespace std;

TESTCASE("FunctionLibrary Test")
{
	SECTION("New FunctionLibrary")
	{
		auto lib = FunctionLibrary::GetFrom(".");
		vector<string> package
		{
			"Basic",
		};
		auto funcDef = "int One()\n"
					   "{\n"
					   "	return 1;\n"
					   "}\n";
		istringstream is(funcDef);
		auto reader = FuncsDefReader(make_unique<istringstream>(move(is)));
		lib.Add(package, move(reader), "math one");
		ASSERT(lib._index._diskBtree->Count() == 1);
	}

	SECTION("Read FunctionLibrary")
	{
		auto lib = FunctionLibrary::GetFrom(".");
		printf("func count %d\n", lib._index._diskBtree->Count());
		ASSERT(lib._index._diskBtree->Count() == 1);
		printf("start invoke\n");
		auto f = FuncType("int", "One", {}, {"Basic"});
		printf("invoke func key %s\n", f.ToKey().c_str());
		auto r = lib.Invoke(f, JsonObject());
		ASSERT(r.IsNumber());
		ASSERT(r.GetNumber() == 1);
	}
}

DEF_TEST_FUNC(TestFunctionLibrary)
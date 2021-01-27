#include "../TestFrame/FlyTest.hpp"
#include "../TestFrame/Util.hpp"
#include "Util.hpp"
#include <sstream>
#define private public
#include "../FunctionLibrary.hpp"

using namespace ::Test;
using namespace FuncLib;
using namespace FuncLib::Test;
using namespace std;

pair<vector<string>, FuncsDefReader> MakeFuncDefReader(string funcDef)
{
	vector<string> package{ "Basic", };
	istringstream is(funcDef);
	auto reader = FuncsDefReader(make_unique<istringstream>(move(is)));
	return pair(move(package), move(reader));
}

TESTCASE("FunctionLibrary Test")
{
	SECTION("New FunctionLibrary")
	{
		auto lib = FunctionLibrary::GetFrom(".");
		auto funcDef = "int Zero()\n"
					   "{\n"
					   "	return 0;\n"
					   "}\n";
		auto [package, reader] = MakeFuncDefReader(funcDef);
		lib.Add(package, move(reader), "math zero");
		ASSERT(lib._index._diskBtree->Count() == 1);
	}

	SECTION("Read FunctionLibrary")
	{
		auto lib = FunctionLibrary::GetFrom(".");
		printf("func count %d\n", lib._index._diskBtree->Count());
		ASSERT(lib._index._diskBtree->Count() == 1);
		printf("start invoke\n");
		auto f = FuncType("int", "Zero", {}, {"Basic"});
		printf("invoke func key %s\n", f.ToKey().c_str());
		auto r = lib.Invoke(f, JsonObject());
		ASSERT(r.IsNumber());
		ASSERT(r.GetNumber() == 0);
	}

	SECTION("Add func to stored FunctionLibrary then invoke")
	{
		array<pair<string, int>, 10> funcInfo
		{
			pair<string, int>("One", 1),
			{ "Two", 2 },
			{ "Three", 3},
			{ "Four", 4 },
			{ "Five", 5 },
			{ "Six", 6 },
			{ "Seven", 7 },
			{ "Eight", 8 },
			{ "Nine", 9 },
			{ "Ten", 10 },
		};

		auto lib = FunctionLibrary::GetFrom(".");

		for (auto& i : funcInfo)
		{
			string defTemplate = "int Name()\n"
						"{\n"
						"	return Num;\n"
						"}\n";
			
			defTemplate.replace(defTemplate.find("Name"), 4, i.first)
					   .replace(defTemplate.find("Num"), 3, to_string(i.second));
			auto [package, reader] = MakeFuncDefReader(move(defTemplate));
			lib.Add(package, move(reader), "math");
		}

		for (auto& i : funcInfo)
		{
			auto f = FuncType("int", i.first, {}, {"Basic"});
			auto r = lib.Invoke(f, JsonObject());
			ASSERT(r.IsNumber());
			ASSERT(r.GetNumber() == i.second);
		}
	}

	SECTION("ModifyPackage")
	{
		auto lib = FunctionLibrary::GetFrom(".");
		auto f = FuncType("int", "One", {}, {"Basic"});
		ASSERT(lib.Contains(f));
		lib.ModifyPackageOf(f, { "Math" });
		ASSERT(lib.Contains(FuncType("int", "One", {}, {"Math"})));
	}
}

void TestFunctionLibrary(bool executed)
{
	Cleaner c1("func.idx"), c2("func_bin.lib");
	if (executed)
	{
		allTest();
	}
	_tests_.clear();
}
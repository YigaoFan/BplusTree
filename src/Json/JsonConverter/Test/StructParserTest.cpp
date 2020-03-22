#include <string_view>
#include "../../../TestFrame/FlyTest.hpp"
#include "../StructParser.hpp"
#include "../DeserialFunctionGenerator.hpp"
using namespace Json::JsonConverter;
using ::std::string_view;
using namespace ::std;

TESTCASE("StructParser test")
{
	vector<string_view> strs
	{
		"  struct  A {",
		"int i; double  d;",
		"string s; }",
	};
	auto functionDef = GenerateDeserializerForStruct(strs);
	for (auto& l : functionDef)
	{
		cout << l << endl;
	}
	auto def = ParseStruct(strs);
	ASSERT(def.TypeName() == "A");
	int i;
	string type;
	string name;

	i = 0;
	type = "int";
	name = "i";
	ASSERT(def.GetType(i) == type);
	ASSERT(def.GetType(name) == type);
	ASSERT(def.GetName(i) == name);
	
	i = 1;
	type = "double";
	name = "d";
	ASSERT(def.GetType(i) == type);
	ASSERT(def.GetType(name) == type);
	ASSERT(def.GetName(i) == name);

	i = 2;
	type = "string";
	name = "s";
	ASSERT(def.GetType(i) == type);
	ASSERT(def.GetType(name) == type);
	ASSERT(def.GetName(i) == name);

	// TODO User-defined stuct data member
}

void structParserTest(bool executed)
{
	if (executed)
	{
		allTest();
	}

	_tests_.~vector();
}

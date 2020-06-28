#include <string_view>
#include "../../../TestFrame/FlyTest.hpp"
#include "../StructParser.hpp"
#include "../DeserialFunctionGenerator.hpp"
#include "../SerialFunctionGenerator.hpp"
#include "../JsonConverter.hpp"
#include "../../Parser.hpp"
using namespace Json::JsonConverter;
using namespace Json;
using ::std::string_view;
using namespace ::std;

TESTCASE("StructParser test")
{
	map<string, int> m
	{
		{"a", 1},
		{"b", 2},
		{"c", 3},
	};
	auto j = Serialize(m);
	cout << j.ToString() << endl;
	auto ds = Deserialize<decltype(m)>(j);

	vector<int> v{ 1, 2, 3 };
	auto json = Serialize(v);
	cout << json.ToString() << endl;

	vector<string_view> strs
	{
		"  struct  A {",
		"int i; double  d;",
		"string s; }",
	};
	auto serDef = GenerateStructSerializer(strs);
	for (auto& l : serDef)
	{
		cout << l << endl;
	}

	auto functionDef = GenerateStructDeserializer(strs);
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
}

void structParserTest(bool executed)
{
	if (executed)
	{
		allTest();
	}

	_tests_.clear();
}

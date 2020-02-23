#include "../../TestFrame/FlyTest.hpp"
#include "../Parser.hpp"
using namespace Json;

TESTCASE("Parser test")
{
	auto parse = Json::Parse;
	// TODO add test code 2.23
	// TODO see MoliY's blog
	SECTION("Parse object")
	{
		auto simpleObjStr = "{ \"a\": 1 }";
		auto simpleObjJson = parse(simpleObjStr);
		ASSERT(simpleObjJson.IsObject() && 
				simpleObjJson["a"].IsNumber() && 
				simpleObjJson["a"].GetNumber() == 1);
		
		auto nestObjStr = "{"
								"\"a\": {"
									"\"First\": \"Hello world\","
								"},"
								"\"b\": 1.23"
							"}";
		auto nestObjJson = parse(nestObjStr);
		ASSERT(nestObjJson.IsObject());
		ASSERT(nestObjJson["a"].IsObject());
		ASSERT(nestObjJson["a"]["First"].IsString() && nestObjJson["a"]["First"].GetString() == "Hello world");
		ASSERT(nestObjJson["b"].IsNumber() && nestObjJson["b"].GetNumber() == 1.23);
	}

	SECTION("Parse array")
	{
		auto jsonStr = "[1, 2, 3, ]";
		auto json = parse(jsonStr);
		ASSERT(json.IsArray());
		ASSERT(json[0].IsNumber() && json[0].GetNumber() == 1);
		ASSERT(json[1].IsNumber() && json[1].GetNumber() == 2);
		ASSERT(json[2].IsNumber() && json[2].GetNumber() == 3);

		auto diffInArrayStr = "[\"a\", 1.23, false, null, {\"First\": \"Hello world\" }";
		auto diffArrayJson = parse(diffInArrayStr);
		ASSERT(diffArrayJson.IsArray());
		ASSERT(diffArrayJson[0].IsString() && json[0].GetString() == "a");
		ASSERT(diffArrayJson[1].IsNumber() && json[1].GetNumber() == 1.23);
		ASSERT(diffArrayJson[2].IsBool() && json[2].GetBool() == false);
		ASSERT(diffArrayJson[3].IsNull());
		ASSERT(diffArrayJson[4].IsObject() && 
				json[4]["First"].IsString() && 
				json[4]["First"].GetString() == "Hello world");
	}

	SECTION("Parse number")
	{
		auto jsonStr = "1.234";
		auto json = parse(jsonStr);
		// Compare code below not standard
		ASSERT(json.IsNumber() && json.GetNumber() == 1.23);
	}

	SECTION("Parse string")
	{
		auto jsonStr = "Hello world";
		auto json = parse(jsonStr);
		ASSERT(json.IsString() && json.GetString() == jsonStr);
	}

	SECTION("Parse bool")
	{
		auto jsonStr = "false";
		auto json = parse(jsonStr);
		ASSERT(json.IsBool() && json.IsFalse());
	}

	SECTION("Parse null")
	{
		auto jsonStr = "null";
		auto json = parse(jsonStr);
		ASSERT(json.IsNull());
	}
}

void parserTest()
{
	allTest();
}

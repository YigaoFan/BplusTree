#include <string>
#include "../../TestFrame/FlyTest.hpp"
#include "../../Basic/Exception.hpp"
#include "../Parser.hpp"
using namespace Json;
using namespace Basic;
using ::std::string;

TESTCASE("Parser test")
{
	auto parse = Json::Parse;
	// TODO test wrong case
	// TODO see MoliY's blog
	SECTION("Parse object")
	{
		SECTION("Simple object")
		{
			string jsonStr = "{ \"a\": 1 }";
			auto json = parse(jsonStr);
			ASSERT(json.IsObject() && json["a"].IsNumber() &&
				json["a"].GetNumber() == 1);
			ASSERT_THROW(InvalidAccessException, json[0]);
			ASSERT(json.Count() == 1);
			auto s = json.ToString();
		}
		
		SECTION("Nested object object")
		{
			auto jsonStr = "{"
				"\"a\": {"
				"\"First\": \"Hello world\","
				"},"
				"\"b\": 1.23"
				"}";
			auto json = parse(jsonStr);
			ASSERT(json.IsObject());
			ASSERT(json["a"].IsObject());
			ASSERT(json["a"]["First"].IsString() && json["a"]["First"].GetString() == "Hello world");
			ASSERT(json["b"].IsNumber() && json["b"].GetNumber() == 1.23);
			auto s = json.ToString();
		}

		SECTION("Nested array object")
		{
			auto jsonStr = "{"
				"\"a\": [1, true, {\"Hello\": \"World\"}],"
				"\"b\": 1.23"
				"}";
			auto json = parse(jsonStr);
			ASSERT(json.IsObject());
			ASSERT(json["a"].IsArray());
			ASSERT(json["a"][0].IsNumber() && json["a"][0].GetNumber() == 1);
			ASSERT(json["a"][1].IsBool() && json["a"][1].GetBool() == true);
			ASSERT(json["a"][2].IsObject() && json["a"][2]["Hello"].IsString() && 
				json["a"][2]["Hello"].GetString() == "World");
			ASSERT(json["b"].IsNumber() && json["b"].GetNumber() == 1.23);
		}
	}

	SECTION("Parse array")
	{
		SECTION("Num array")
		{
			auto jsonStr = "[1, 2, 3, ]";
			auto json = parse(jsonStr);
			ASSERT(json.IsArray());
			ASSERT(json[0].IsNumber() && json[0].GetNumber() == 1);
			ASSERT(json[1].IsNumber() && json[1].GetNumber() == 2);
			ASSERT(json[2].IsNumber() && json[2].GetNumber() == 3);
			ASSERT(json.Count() == 3);
			auto s = json.ToString();
		}

		SECTION("Various element array")
		{
			auto jsonStr = "[0, true, null, ]";
			auto json = parse(jsonStr);
			ASSERT(json.IsArray());
			ASSERT(!json.IsObject());
			ASSERT(json[0].IsNumber() && json[0].GetNumber() == 0);
			ASSERT(json[1].IsBool() && json[1].GetBool() == true);
			ASSERT(json[2].IsNull());
			ASSERT(json.Count() == 3);
			auto s = json.ToString();
		}

		SECTION("Nested object array")
		{
			auto jsonStr = "[\"a\", 1.23, false, null, {\"First\": \"Hello world\" }]";
			auto json = parse(jsonStr);
			ASSERT(json.IsArray());
			ASSERT(json[0].IsString() && json[0].GetString() == "a");
			ASSERT(json[1].IsNumber() && json[1].GetNumber() == 1.23);
			ASSERT(json[2].IsBool() && json[2].GetBool() == false);
			ASSERT(json[3].IsNull());
			ASSERT(json[4].IsObject() &&
				json[4]["First"].IsString() &&
				json[4]["First"].GetString() == "Hello world");
		}

		SECTION("Nested array array")
		{
			auto jsonStr = "[\"a\", 1.23, false, null, [\"First\"]]";
			auto json = parse(jsonStr);
			ASSERT(json.IsArray());
			ASSERT(json[0].IsString() && json[0].GetString() == "a");
			ASSERT(json[1].IsNumber() && json[1].GetNumber() == 1.23);
			ASSERT(json[2].IsBool() && json[2].GetBool() == false);
			ASSERT(json[3].IsNull());
			ASSERT(json[4].IsArray() && json[4][0].IsString() &&
				json[4][0].GetString() == "First");
		}
	}

	SECTION("Parse number")
	{
		auto jsonStr = "1.234";
		auto json = parse(jsonStr);
		// Compare float below maybe not standard
		ASSERT(json.IsNumber() && json.GetNumber() == 1.234);
		auto str = json.ToString();
		//ASSERT(str == jsonStr);
	}

	SECTION("Parse string")
	{
		auto jsonStr = "\"Hello world\"";
		auto json = parse(jsonStr);
		ASSERT(json.IsString() && json.GetString() == "Hello world");
		ASSERT(json.ToString() == jsonStr);
	}

	SECTION("Parse bool")
	{
		auto jsonStr = "false";
		auto json = parse(jsonStr);
		ASSERT(json.IsBool() && json.IsFalse());
		ASSERT(json.ToString() == jsonStr);
	}

	SECTION("Parse null")
	{
		auto jsonStr = "null";
		auto json = parse(jsonStr);
		ASSERT(json.IsNull());
		ASSERT(json.ToString() == jsonStr);
	}
}

void parserTest(bool executed)
{
	if (executed)
	{
		allTest();
	}

	_tests_.clear();
}

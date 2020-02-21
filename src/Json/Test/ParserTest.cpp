#include "../../TestFrame/FlyTest.hpp"
#include "../Parser.hpp"
using namespace Json;

TESTCASE("Parser test")
{
	auto jsonStr = "{ \"a\": 1 }";
	auto json = Json::Parse(jsonStr);
	// TODO see MoliY's blog
	SECTION("Parse object")
	{

	}

	SECTION("Parse array")
	{
		
	}

	SECTION("Parse number")
	{
		
	}

	SECTION("Parse string")
	{
		
	}

	SECTION("Parse bool")
	{
		
	}

	SECTION("Parse null")
	{
		
	}
}

void parserTest()
{
	allTest();
}

#pragma once

#include "../../TestFrame/FlyTest.hpp"
#include "../Parser.hpp"
using namespace Json;

TESTCASE("Parser test") {
	auto jsonStr = "{ \"a\": 1 }";
	auto json = Parser::parse(jsonStr);
}

void
parserTest()
{
	allTest();
}

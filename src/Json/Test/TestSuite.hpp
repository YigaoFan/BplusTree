#pragma once
#include "../JsonConverter/Test/TestSuite.hpp"
extern void parserTest(bool executed);

namespace Json::Test
{
	void AllTest(bool executed)
	{
		parserTest(executed);
		JsonConverterTest::AllTest(executed);
	}
}
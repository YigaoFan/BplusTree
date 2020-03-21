#pragma once
#include "../JsonConverter/Test/TestSuite.hpp"
extern void parserTest(bool executed);

namespace JsonTest
{
	void AllTest(bool executed)
	{
		parserTest(false);
		JsonConverterTest::AllTest(executed);
	}
}
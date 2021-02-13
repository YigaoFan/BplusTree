#pragma once
#include "../JsonConverter/Test/TestSuite.hpp"
extern void TestParser(bool executed);

namespace Json::Test
{
	void AllTest(bool executed)
	{
		TestParser(executed);
		JsonConverterTest::AllTest(executed);
	}
}
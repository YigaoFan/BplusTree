#pragma once
extern void TestWordEnumerator(bool executed);
extern void TestStructParser(bool executed);

namespace JsonConverterTest
{
	void AllTest(bool executed)
	{
		TestWordEnumerator(executed);
		TestStructParser(executed);
	}
}
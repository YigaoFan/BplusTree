#pragma once
extern void wordEnumeratorTest(bool executed);
extern void structParserTest(bool executed);

namespace JsonConverterTest
{
	void AllTest(bool executed)
	{
		wordEnumeratorTest(executed);
		structParserTest(executed);
	}
}
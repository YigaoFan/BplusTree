#include <string_view>
#include "../../../TestFrame/FlyTest.hpp"
#include "../WordEnumerator.hpp"
using namespace Json::JsonConverter;
using ::std::string_view;

TESTCASE("WordEnumerator test")
{
	vector<string_view> strs
	{
		"  struct  A {",
		"int i; double d;",
		"string s; }",
	};
	auto e = WordEnumerator(strs, ' ');
	ASSERT(e.MoveNext());
	ASSERT(e.Current() == "struct"); // 1
	ASSERT(e.MoveNext());
	ASSERT(e.Current() == "A"); // 2
	ASSERT(e.MoveNext());
	ASSERT(e.Current() == "{"); // 3

	e.ChangeSeparator(';');
	ASSERT(e.MoveNext());
	ASSERT(e.Current() == "int i"); // 4
	ASSERT(e.MoveNext());
	ASSERT(e.Current() == " double d"); // 5
	ASSERT(e.MoveNext());
	ASSERT(e.Current() == "string s"); // 6
	ASSERT(e.Current() == "string s"); // Multiple read
	ASSERT(e.MoveNext());
	ASSERT(e.Current() == " }"); // 7
	ASSERT(e.CurrentIndex() == 6);
	ASSERT(!e.MoveNext());
}

void wordEnumeratorTest(bool executed)
{
	if (executed)
	{
		allTest();
	}

	_tests_.clear();
}

#define MEMO_CHECK true
#if MEMO_CHECK
#include <crtdbg.h>
#endif

#define BTREE_TEST_SWITCH true
#define JSON_TEST_SWITCH true

#if BTREE_TEST_SWITCH
#include "Btree/Test/TestSuite.hpp"
#define BTREE_TEST(FUNC) FUNC
#else
#define BTREE_TEST(FUNC)
#endif

#if JSON_TEST_SWITCH
#include "Json/Test/TestSuite.hpp"
#define JSON_TEST(FUNC) FUNC
#else
#define JSON_TEST(FUNC)
#endif

#include <iostream>
#include "Btree/Test/MemoryMemo.hpp"

int main()
{
#if MEMO_CHECK
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
#endif
	BTREE_TEST(CollectionsTest::AllTest(false));
	JSON_TEST(JsonTest::AllTest(true));

#if MEMO_CHECK
	 _CrtDumpMemoryLeaks();
#endif
	return 0;
}



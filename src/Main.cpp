#define MEMO_CHECK false
#if MEMO_CHECK
#include <crtdbg.h>
#endif

#define COLLECTIONS_TEST_SWITCH true
#define JSON_TEST_SWITCH true
#define FUNC_LIB_TEST_SWITCH true

#if COLLECTIONS_TEST_SWITCH
#include "Btree/Test/TestSuite.hpp"
#define COLLECTIONS_TEST(FUNC) FUNC
#else
#define COLLECTIONS_TEST(FUNC)
#endif

#if JSON_TEST_SWITCH
#include "Json/Test/TestSuite.hpp"
#define JSON_TEST(FUNC) FUNC
#else
#define JSON_TEST(FUNC)
#endif

#if FUNC_LIB_TEST_SWITCH
#include "FuncLib/Test/TestSuite.hpp"
#define FUNC_LIB_TEST(FUNC) FUNC
#else
#define FUNC_LIB_TEST(FUNC)
#endif

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
	COLLECTIONS_TEST(CollectionsTest::AllTest(true));
	JSON_TEST(JsonTest::AllTest(true));
	FUNC_LIB_TEST(FuncLibTest::AllTest(true));

#if MEMO_CHECK
	 _CrtDumpMemoryLeaks();
#endif
	return 0;
}



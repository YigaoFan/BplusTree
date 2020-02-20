#define MEMO_CHECK false
#if MEMO_CHECK
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#define BTREE_TEST_SWITCH true
#define JSON_TEST_SWITCH false

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

int main()
{
	//new int(2); // For check MEMO_CHECK mechanism work or not
	BTREE_TEST(CollectionsTest::AllTest());
	JSON_TEST(JsonTest::allTest());

#if MEMO_CHECK
	// Remember free the test frame related memory
	 _CrtDumpMemoryLeaks();
#endif
	return 0;
}



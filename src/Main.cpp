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
	BTREE_TEST(CollectionsTest::AllTest());
	JSON_TEST(JsonTest::allTest());

#ifdef _MSC_VER
	// free the test frame related memory
	// _CrtDumpMemoryLeaks();
#endif
	return 0;
}



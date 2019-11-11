#define BTREE_TEST_BUTTON true
#define JSON_TEST_BUTTON false

#if BTREE_TEST_BUTTON
#include "Btree/Test/TestSuite.hpp"
#define BTREE_TEST(FUNC) FUNC
#else
#define BTREE_TEST(FUNC)
#endif

#if JSON_TEST_BUTTON
#include "Json/Test/TestSuite.hpp"
#define JSON_TEST(FUNC) FUNC
#else
#define JSON_TEST(FUNC)
#endif

int
main()
{
	BTREE_TEST(BtreeTest::allTest());
	JSON_TEST(JsonTest::allTest());

	return 0;
}
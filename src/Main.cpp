#include "Btree/Test/TestSuite.hpp"
#include "Json/Test/TestSuite.hpp"

int
main()
{
	BtreeTest::allTest();
	JsonTest::allTest();

	return 0;
}
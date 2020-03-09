extern void btreeTest(bool isSkipped);
extern void liteVectorTest(bool isSkipped);
extern void elementsTest(bool isSkipped);

namespace CollectionsTest
{
	void AllTest(bool isSkipped)
	{
		liteVectorTest(isSkipped);
		btreeTest(isSkipped);
		elementsTest(isSkipped);
	}
}
extern void btreeTest(bool isSkipped);
extern void liteVectorTest(bool isSkipped);

namespace CollectionsTest
{
	void AllTest(bool isSkipped)
	{
		liteVectorTest(true); // If not call, _test_ vector memory will not free before main end
		btreeTest(isSkipped);
	}
}
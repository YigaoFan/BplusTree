// extern void elementTest();
// extern void nodeTest();
extern void btreeTest();
extern void liteVectorTest();

namespace CollectionsTest
{
	void AllTest() 
	{
		// elementTest();
		// nodeTest();
		liteVectorTest(); // If not call, _test_ vector memory will not free before main end
		btreeTest();
	}
}
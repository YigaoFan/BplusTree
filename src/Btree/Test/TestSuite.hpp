extern void btreeTest(bool executed);
extern void liteVectorTest(bool executed);
extern void elementsTest(bool executed);

namespace Collections::Test
{
	void AllTest(bool executed)
	{
		liteVectorTest(executed);
		btreeTest(executed);
		elementsTest(executed);
	}
}
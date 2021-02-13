extern void TestBtree(bool executed);
extern void TestLiteVector(bool executed);
extern void TestElements(bool executed);

namespace Collections::Test
{
	void AllTest(bool executed)
	{
		TestLiteVector(executed);
		TestBtree(executed);
		TestElements(executed);
	}
}
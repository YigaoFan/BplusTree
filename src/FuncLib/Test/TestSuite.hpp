// extern void diskBtreeTest(bool executed);
extern void fileTest(bool executed);
extern void byteConverterTest(bool executed);
extern void typeConverterTest(bool executed);

namespace FuncLibTest
{
	void AllTest(bool executed)
	{
		// diskBtreeTest(executed);
		fileTest(executed);
		byteConverterTest(executed);
		typeConverterTest(executed);
	}
}
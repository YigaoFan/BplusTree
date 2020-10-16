// extern void diskBtreeTest(bool executed);
extern void fileTest(bool executed);
extern void fileReader_FileWriterTest(bool executed);
extern void byteConverterTest(bool executed);
extern void storageAllocatorTest(bool executed);
extern void fileCacheTest(bool executed);
extern void typeConverterTest(bool executed);

namespace FuncLibTest
{
	void AllTest(bool executed)
	{
		// diskBtreeTest(executed);
		fileTest(executed);
		fileCacheTest(executed);
		storageAllocatorTest(executed);
		typeConverterTest(executed);
		fileReader_FileWriterTest(executed);
		byteConverterTest(executed);
	}
}
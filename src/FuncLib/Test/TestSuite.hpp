#pragma once
extern void fileTest(bool executed);
extern void fileReader_ObjectWriterTest(bool executed);
extern void byteConverterTest(bool executed);
extern void storageAllocatorTest(bool executed);
extern void fileCacheTest(bool executed);
extern void typeConverterTest(bool executed);
extern void compileTest(bool executed);
extern void TestLabelNode(bool executed);

namespace FuncLib::Test
{
	void AllTest(bool executed)
	{
		fileTest(false);
		fileCacheTest(false);
		storageAllocatorTest(false);
		typeConverterTest(false);
		fileReader_ObjectWriterTest(false);
		byteConverterTest(false);
		compileTest(false);
		TestLabelNode(executed);
	}
}
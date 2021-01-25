#pragma once
extern void TestFile(bool executed);
extern void TestFileReaderObjectBytes(bool executed);
extern void TestStorageAllocator(bool executed);
extern void TestFileCache(bool executed);
extern void TestCompile(bool executed);
extern void TestLabelNode(bool executed);
extern void TestObjectRelationTree(bool executed);
extern void typeConverterTest(bool executed);
extern void byteConverterTest(bool executed);
extern void TestFunctionLibrary(bool executed);

namespace FuncLib::Test
{
	void AllTest(bool executed)
	{
		TestLabelNode(executed);
		TestObjectRelationTree(executed);
		TestFileReaderObjectBytes(executed);
		TestFileCache(executed);
		TestCompile(executed);
		TestStorageAllocator(executed);
		TestFile(executed);
		TestFunctionLibrary(executed);
		// 有时间可以整理下面这两个
		typeConverterTest(false);
		byteConverterTest(false);
	}
}
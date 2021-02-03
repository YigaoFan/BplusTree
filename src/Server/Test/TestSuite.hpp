#pragma once
extern void TestMockServer(bool executed);
extern void TestCmdFunction(bool executed);
extern void TestFuncLibWorker(bool executed);

namespace Server::Test
{
	void AllTest(bool executed)
	{
		TestFuncLibWorker(false);
		TestMockServer(executed);
		TestCmdFunction(false);
		// 之后把这些测试方法的名字都改成大写
	}
}
#pragma once
extern void TestMockServer(bool executed);
extern void TestCmdFunction(bool executed);
extern void TestFuncLibWorker(bool executed);
extern void TestServer(bool executed);

namespace Server::Test
{
	void AllTest(bool executed)
	{
		TestServer(executed);
		TestFuncLibWorker(false);
		// TestMockServer(executed);
		TestCmdFunction(false);
	}
}
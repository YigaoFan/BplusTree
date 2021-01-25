#pragma once
extern void TestServer(bool executed);
extern void TestCmdFunction(bool executed);

namespace Server::Test
{
	void AllTest(bool executed)
	{
		TestServer(false);
		TestCmdFunction(executed);
		// 之后把这些测试方法的名字都改成大写
	}
}
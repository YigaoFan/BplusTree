#pragma once
extern void ServerTest(bool executed);
extern void CmdFunctionTest(bool executed);

namespace Server::Test
{
	void AllTest(bool executed)
	{
		ServerTest(false);
		CmdFunctionTest(executed);
		// 之后把这些测试方法的名字都改成大写
	}
}
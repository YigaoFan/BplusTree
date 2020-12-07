#pragma once
extern void ServerTest(bool executed);

namespace Server::Test
{
	void AllTest(bool executed)
	{
		ServerTest(false);
	}
}
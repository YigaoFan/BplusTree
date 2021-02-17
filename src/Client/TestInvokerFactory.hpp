#pragma once
#include "../RemoteInvokeLib/RemoteInvoker.hpp"

namespace Client
{
	using namespace RemoteInvokeLib;

	RemoteInvoker MakeTestInvoker()
	{
		return RemoteInvoker::New("192.168.10.32", 8888, "yigao fan", "hello world");
	}
}
#pragma once
#include "../RemoteInvokeLib/RemoteInvoker.hpp"

namespace Client
{
	using namespace RemoteInvokeLib;

	RemoteInvoker MakeTestInvoker()
	{
		return RemoteInvoker::New("127.0.0.1", 8888, "yigao fan", "hello world");
	}
}
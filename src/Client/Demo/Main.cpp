#include <string>
#include "../RemoteInvokeLib/RemoteInvoker.hpp"

using namespace RemoteInvokeLib;
using namespace std;

int main()
{
	auto invoker = RemoteInvoker::New("localhost", 8888, "yigaofan", "777");
	auto n = invoker.Invoke<int()>({ "Basic" }, "One");
	auto s = invoker.Invoke<string(string)>({ "Basic" }, "Hello", "fanyigao");
}
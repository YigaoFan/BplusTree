#include <string>
#include "../RemoteInvokeLib/RemoteInvoker.hpp"

using namespace RemoteInvokeLib;
using namespace std;

int main()
{
	auto invoker = RemoteInvoker::New("127.0.0.1", 8888, "yigao fan", "hello world");
	auto n = invoker.Invoke<int()>({ "Basic" }, "One");
	printf("One %d\n", n);
	auto s = invoker.Invoke<string(string)>({ "Basic" }, "Hello", "fanyigao");
	printf("greet %s\n", s.c_str());
}
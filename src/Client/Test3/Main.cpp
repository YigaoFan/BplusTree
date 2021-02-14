#include <array>
#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include "../TestInvokerFactory.hpp"

using namespace Client;
using namespace std;

atomic<int> CompletedCount = 0;

void Func()
{
	auto invoker = MakeTestInvoker();
	constexpr auto a = 10;
	constexpr auto b = 5;
	auto add = [&]
	{
		auto r = invoker.Invoke<int(int, int)>({ "Basic" }, "Add", a, b);
	};

	auto sub = [&]
	{
		auto r = invoker.Invoke<int(int, int)>({ "Basic" }, "Sub", a, b);
	};

	auto div = [&]
	{
		auto r = invoker.Invoke<int(int, int)>({ "Basic" }, "Div", a, b);
	};

	auto mul = [&]
	{
		auto r = invoker.Invoke<int(int, int)>({ "Basic" }, "Mul", a, b);
	};

	array<function<void()>, 4> funcs
	{
		add, sub, div, mul,
	};

	for (auto i = 0; i < 250; ++i)
	{
		for (auto& f : funcs)
		{
			f();
		}
	}
	++CompletedCount;
}

int main()
{
	auto n = 10;
	for (auto i = 0; i < n; ++i)
	{
		thread t(Func);
		t.detach();
	}

	for (;;)
	{
		if (CompletedCount == n)
		{
			break;
		}
	}
}
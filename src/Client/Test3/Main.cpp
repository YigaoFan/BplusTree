#include <array>
#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include "../Basic/Debug.hpp"
#include "../TestInvokerFactory.hpp"

using namespace Client;
using namespace std;
using Basic::Assert;

atomic<int> CompletedCount = 0;

void Func()
{
	auto invoker = MakeTestInvoker();
	constexpr auto a = 10;
	constexpr auto b = 5;
	auto add = [&]
	{
		auto r = invoker.Invoke<int(int, int)>({ "Basic" }, "Add", a, b);
		Assert(r == a + b, "add result not correct");
	};

	auto sub = [&]
	{
		auto r = invoker.Invoke<int(int, int)>({ "Basic" }, "Sub", a, b);
		Assert(r == a - b, "sub result not correct");
	};

	auto div = [&]
	{
		auto r = invoker.Invoke<int(int, int)>({ "Basic" }, "Div", a, b);
		Assert(r == a / b, "div result not correct");
	};

	auto mul = [&]
	{
		auto r = invoker.Invoke<int(int, int)>({ "Basic" }, "Mul", a, b);
		Assert(r == a * b, "mul result not correct");
	};

	array<function<void()>, 4> funcs
	{
		add, sub, div, mul,
	};

	for (auto i = 0; i < 250; ++i)
	{
		// printf("round %d\n", i);
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
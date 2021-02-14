#include <array>
#include <functional>
#include "../Basic/Debug.hpp"
#include "../TestInvokerFactory.hpp"

using namespace std;
using namespace Client;
using Basic::Assert;

int main()
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
		for (auto& f : funcs)
		{
			f();
		}
	}
}
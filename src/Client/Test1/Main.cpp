#include <string>
#include "../Basic/Debug.hpp"
#include "../TestInvokerFactory.hpp"

using namespace std;
using namespace Client;
using Basic::Assert;

int main()
{
	auto invoker = MakeTestInvoker();
	constexpr auto n = 1048576;
	auto genStr = [](char c)
	{
		string s;
		s.reserve(n);
		for (auto i = 0; i < n; ++i)
		{
			s.push_back(c);
		}
		return s;
	};
	auto concat = [&]
	{
		auto s1 = genStr('s');
		auto s2 = genStr('t');
		auto r = invoker.Invoke<string(string, string)>({ "Basic" }, "Concat", s1, s2);
		return r;
	};
	auto s = concat();
	Assert(s.size() == n * 2, "concat result size not correct");
	for (auto i = 0; i < 2 * n; ++i)
	{
		auto c = s[i];
		if (i < n)
		{
			Assert(c == 's', "first half contain wrong char");
		}
		else
		{
			Assert(c == 't', "second half contain wrong char");
		}
	}
}
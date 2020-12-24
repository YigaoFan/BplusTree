// Cmake 里面如何设置构建多个 exe

#include "StringMatcher.hpp"
using namespace Cmd;

int main()
{
	vector<string> cmds
	{
		"Search",
		"Remove",
		"Add",
	};

	auto m = StringMatcher(move(cmds));
	printf("match result: %s", m.Match("Ad")[0].c_str());

	return 0;
}

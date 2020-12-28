#define IS_WINDOWS false
#define IS_UNIX true

#if IS_WINDOWS
#include "Main_Win.hpp"
#elif IS_UNIX
#include "Main_macOS.hpp"
#else
#error No support platform
#endif

int main()
{
	return UI_Main();
	// vector<string> cmds
	// {
	// 	"Search",
	// 	"Remove",
	// 	"Add",
	// };

	// auto m = StringMatcher(move(cmds));
	// printf("match result: %s", m.Match("Ad")[0].c_str());

	return 0;
}

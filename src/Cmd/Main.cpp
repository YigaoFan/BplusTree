#define IS_WINDOWS false
#define IS_MACOS true

#if IS_WINDOWS
#include "Main_Win.hpp"
#elif IS_MACOS
#include "Main_macOS.hpp"
#else
#error No support platform
#endif

int main()
{
	return UI_Main();
}

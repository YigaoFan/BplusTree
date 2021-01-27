#pragma once
#include <cstdio>

namespace Test
{
	template <typename... Ts>
	void log(char const* format, Ts... ts)
	{
		printf(format, ts...);
	}

	struct Cleaner
	{
		char const *Path;

		Cleaner(char const *path) : Path(path) {}
		~Cleaner()
		{
			remove(Path);
		}
	};
}
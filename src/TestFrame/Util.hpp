#pragma once
#include <cstdio>

namespace Test
{
	template <typename... Ts>
	void log(char const* format, Ts... ts)
	{
		auto f = fopen("cmd.log", "a");
		fprintf(f, format, ts...);
		fclose(f);
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
#pragma once
#include <string>
#include <filesystem>

namespace FuncLib::Test
{
	using ::std::string;
	using ::std::filesystem::path;

	struct Cleaner
	{
		char const* Path;

		Cleaner(char const* path);
		~Cleaner();
	};

	path MakeFilePath(string filename);
}
#pragma once
#include <string>
#include <vector>
#include <filesystem>

namespace FuncLib::Compile
{
	using ::std::string;
	using ::std::vector;
	using ::std::filesystem::remove;

	string RandomString();
	vector<char> ReadFileBytes(char const* filename);

	struct FilesCleaner
	{
		vector<string> _filenames;

		FilesCleaner(vector<string> filenames = {}) : _filenames(move(filenames))
		{ }

		FilesCleaner(string filename) : FilesCleaner(vector{ move(filename) })
		{ }

		FilesCleaner(FilesCleaner&& that) noexcept = default;
		FilesCleaner(FilesCleaner const& that) = delete;

		void Add(string filename)
		{
			_filenames.push_back(move(filename));
		}

		~FilesCleaner()
		{
			for (auto &f : _filenames)
			{
				remove(f);
			}
		}
	};
}
#pragma once
#include <fstream>
#include <vector>
#include "CurrentFile.hpp"

namespace FuncLib
{
	using ::std::move;
	using ::std::ifstream;
	using ::std::vector;

	class FileUtil
	{
	public:
		template <typename T>
		static T Read(uint32_t start, uint32_t size)
		{
			auto p = CurrentFile::GetPath();
			ifstream fs(p, ifstream::binary | ifstream::in);
			char c;
			while (((start--) != 0) && fs.get(c))
			{ }

			char[sizeof(T)] mem;
			T* const p = reinterpret_cast<T*>(&mem[0]);
			for (auto i = 0; i < size && fs.get(c); ++i)
			{
				mem[i] = c;
			}

			return move(*p);
		}

		static vector<char> Read(uint32_t start, uint32_t size)
		{
			auto p = CurrentFile::GetPath();
			ifstream fs(p, ifstream::binary | ifstream::in);
			char c;
			while (((start--) != 0) && fs.get(c))
			{ }

			vector<char> mem;
			mem.reserve(size);

			for (auto i = 0; i < size && fs.get(c); ++i)
			{
				mem.push_back(c);
			}

			return mem;
		}

		template <typename T>
		static void Write(uint32_t start, uint32_t size)
		{
			// TODO
		}
	};
}
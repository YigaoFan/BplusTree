#pragma once
#include <vector>
#include <array>
#include <cstddef>
#include <filesystem>
#include "StaticConfig.hpp"
#include "FileByteMover.hpp"

namespace FuncLib::Store
{
	using ::std::array;
	using ::std::byte;
	using ::std::vector;

	template <typename ForwardIter>
	void Write(path const& filename, pos_int start, ForwardIter begin, ForwardIter end)
	{
		ofstream fs(filename, ofstream::binary);
		fs.seekp(start);
		fs.write((char *)begin, end - begin);
	}

	class FileWriter : protected FileByteMover
	{
	private:
		using Base = FileByteMover;
	public:
		using Base::Base;

		void Write(vector<byte> data)
		{
			Write(*_filename, data.begin(), data.end());
		}

		template <size_t N>
		void Write(array<byte, N> data)
		{
			Write(*_filename, data.begin(), data.end());
		}
	};
}

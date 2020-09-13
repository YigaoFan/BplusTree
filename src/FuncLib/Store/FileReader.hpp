#pragma once
#include <vector>
#include <array>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include "StaticConfig.hpp"
#include "FileByteMover.hpp"

namespace FuncLib::Store
{
	using ::std::array;
	using ::std::byte;
	using ::std::ifstream;
	using ::std::size_t;
	using ::std::vector;
	using ::std::filesystem::path;

	vector<byte> ReadByte(path const& filename, pos_int start, size_t size);

	template <size_t N>
	array<byte, N> ReadByte(path const& filename, pos_int start)
	{
		ifstream fs(filename, ifstream::binary);
		fs.seekg(start);

		array<byte, N> mem;

		if (fs.is_open())
		{
			fs.read(reinterpret_cast<char *>(&mem[0]), N);
		}

		return mem;
	}

	class FileReader : protected FileByteMover
	{
	private:
		using Base = FileByteMover;
	public:
		using Base::Base;

		/// has side effect: move forward size positions
		vector<byte> Read(size_t size);

		/// has side effect: move forward N positions
		template <size_t N>
		array<byte, N> Read()
		{
			auto pos = pos_;
			pos_ += N;
			auto path = GetPath();
			return ReadByte<N>(*path, pos);
		}
	};
}

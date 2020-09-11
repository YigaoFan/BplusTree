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

	vector<byte> Read(path const& filename, pos_int start, size_t size)
	{
		ifstream fs(filename, ifstream::binary);
		fs.seekg(start);

		vector<byte> mem(size);

		if (fs.is_open())
		{
			fs.read(reinterpret_cast<char *>(&mem[0]), size);
		}

		return mem;
	}

	template <size_t N>
	array<byte, N> Read(path const& filename, pos_int start)
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

		vector<byte> Read(size_t size)
		{
			auto p = _pos;
			_pos += size;
			return Read(*_filename, _pos, size);
		}

		template <size_t N>
		array<byte, N> Read()
		{
			auto p = _pos;
			_pos += N;
			return Read<N>(*_filename, p);
		}
	};
	
	FileReader::~FileReader()
	{
	}
}

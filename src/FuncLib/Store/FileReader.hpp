#pragma once
#include <vector>
#include <array>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <memory>
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using ::std::array;
	using ::std::byte;
	using ::std::ifstream;
	using ::std::shared_ptr;
	using ::std::size_t;
	using ::std::vector;
	using ::std::filesystem::path;

	vector<byte> ReadByte(path const& filename, pos_int start, size_t size);

	template <size_t N>
	array<byte, N> ReadByte(path const& filename, pos_int start)
	{
		if constexpr (N == 0)
		{
			return {};
		}

		ifstream fs(filename, ifstream::binary);
		fs.seekg(start);

		array<byte, N> mem;

		if (fs.is_open())
		{
			fs.read(reinterpret_cast<char *>(&mem[0]), N);
		}

		return mem;
	}

	class File;
	// 这个应该成为一个接口或者 concept，方便测试
	class FileReader
	{
	private:
		File* _file;
		shared_ptr<path> _filename;
		pos_int _pos;
	public:
		FileReader(shared_ptr<path> filename, pos_int startPos);
		/// if you want to use File pointer in the read process, you should chose this constructor
		FileReader(File* file, shared_ptr<path> filename, pos_int startPos);
		/// has side effect: move forward size positions
		vector<byte> Read(size_t size);
		File* GetLessOwnershipFile() const;

		/// has side effect: move forward N positions
		template <size_t N>
		array<byte, N> Read()
		{
			auto pos = _pos;
			_pos += N;
			return ReadByte<N>(*_filename, pos);
		}
	};
}

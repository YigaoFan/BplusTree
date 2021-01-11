#pragma once
#include <vector>
#include <array>
#include <cstddef>
#include <filesystem>
#include <fstream>
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

	vector<byte> ReadByte(ifstream* readStream, pos_int start, size_t size);

	template <size_t N>
	array<byte, N> ReadByte(ifstream* readStream, pos_int start)
	{
		if constexpr (N == 0)
		{
			return {};
		}

		readStream->seekg(start);
		array<byte, N> mem;
		readStream->read(reinterpret_cast<char *>(&mem[0]), N);

		return mem;
	}

	class File;
	class FileReader
	{
	private:
		File* _file;
		ifstream _readStream;
		pos_int _pos;
	public:
		static FileReader MakeReader(File *file, path const &filename, pos_int pos);
		FileReader(ifstream readStream, pos_int startPos);
		/// if you want to use File pointer in the read process, you should chose this constructor
		FileReader(File* file, ifstream readStream, pos_int startPos);
		/// has side effect: move forward size positions
		vector<byte> Read(size_t size);
		File* GetLessOwnershipFile() const;

		/// Side effect: move forward N positions
		template <size_t N>
		array<byte, N> Read()
		{
			auto pos = _pos;
			_pos += N;
			return ReadByte<N>(&_readStream, pos);
		}
	};
}

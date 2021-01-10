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
	using ::std::istream;
	using ::std::shared_ptr;
	using ::std::size_t;
	using ::std::unique_ptr;
	using ::std::vector;
	using ::std::filesystem::path;

	vector<byte> ReadByte(istream* readStream, pos_int start, size_t size);

	template <size_t N>
	array<byte, N> ReadByte(istream* readStream, pos_int start)
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
	// 这个应该成为一个接口或者 concept，方便测试
	class FileReader
	{
	private:
		File* _file;
		unique_ptr<istream> _readStream;
		pos_int _pos;
	public:
		static FileReader MakeReader(File *file, path const &filename, pos_int pos);
		FileReader(unique_ptr<istream> readStream, pos_int startPos);
		/// if you want to use File pointer in the read process, you should chose this constructor
		FileReader(File* file, unique_ptr<istream> readStream, pos_int startPos);
		/// has side effect: move forward size positions
		vector<byte> Read(size_t size);
		File* GetLessOwnershipFile() const;

		/// Side effect: move forward N positions
		template <size_t N>
		array<byte, N> Read()
		{
			auto pos = _pos;
			_pos += N;
			return ReadByte<N>(_readStream.get(), pos);
		}
	};
}

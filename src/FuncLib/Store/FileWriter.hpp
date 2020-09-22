#pragma once
#include <cstddef>
#include <filesystem>
#include <memory>
#include <vector>
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using ::std::byte;
	using ::std::shared_ptr;
	using ::std::size_t;
	using ::std::vector;
	using ::std::filesystem::path;

	void WriteByte(path const& filename, pos_int start, char const* begin, size_t size);

	// writer 上内存要不要分区？
	class FileWriter
	{
	private:
		pos_int _pos;
		shared_ptr<path> _filename;
		vector<char> _buffer;
	public:
		FileWriter(shared_ptr<path> filename);
		FileWriter(shared_ptr<path> filename, pos_int startPos);
		// void CurrentPos(pos_int pos);
		/// get the next char position
		pos_int CurrentPos() const;
		void Write(char const* begin, size_t size);
		// size_t BufferSize() const;
		void Flush();
		void WriteBlank(size_t count);
		~FileWriter();
	};
}
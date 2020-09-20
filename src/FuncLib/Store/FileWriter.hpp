#pragma once
#include <cstddef>
#include <filesystem>
#include <memory>
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using ::std::byte;
	using ::std::shared_ptr;
	using ::std::size_t;
	using ::std::filesystem::path;

	void WriteByte(path const& filename, pos_int start, char const* begin, size_t size);

	class FileWriter
	{
	private:
		pos_int _pos;
		shared_ptr<path> _filename;
	public:
		FileWriter(shared_ptr<path> filename);
		FileWriter(shared_ptr<path> filename, pos_int startPos);
		void Pos(pos_int pos);
		pos_int Pos() const;
		void Write(char const* begin, size_t size);
		size_t BufferSize() const;
		void Flush();
		void AppendBlank(size_t count);
		~FileWriter();
	};
}
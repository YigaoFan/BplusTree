#pragma once
#include <cstddef>
#include <filesystem>
#include "StaticConfig.hpp"
#include "FileByteMover.hpp"

namespace FuncLib::Store
{
	using ::std::byte;
	using ::std::filesystem::path;

	void WriteByte(path const& filename, pos_int start, char const* begin, char const* end);

	class FileWriter : protected FileByteMover
	{
	private:
		using Base = FileByteMover;
	public:
		using Base::Base;

		void Write(char const* begin, char const* end);
	};
}
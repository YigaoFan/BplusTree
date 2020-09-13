#pragma once
#include <memory>
#include <filesystem>
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using ::std::shared_ptr;
	using ::std::filesystem::path;

	class File;

	class FileByteMover
	{
	protected:
		shared_ptr<File> file_;
		pos_int pos_;
	public:
		FileByteMover(shared_ptr<File> file, pos_int startPos);
		shared_ptr<File> GetFile() const;

	protected:
		shared_ptr<path> GetPath() const;
	};
}

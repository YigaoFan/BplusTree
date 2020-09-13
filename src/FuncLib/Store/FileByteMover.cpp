#include "File.hpp"
#include "FileByteMover.hpp"

namespace FuncLib::Store
{
	FileByteMover::FileByteMover(shared_ptr<File> file, pos_int startPos)
		: file_(file), pos_(startPos)
	{ }

	shared_ptr<File> FileByteMover::GetFile() const
	{
		return file_;
	}

	shared_ptr<path> FileByteMover::GetPath() const
	{
		return file_->Path();
	}
}
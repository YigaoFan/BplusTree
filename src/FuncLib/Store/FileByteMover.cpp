#include "FileByteMover.hpp"

namespace FuncLib::Store
{
	FileByteMover::FileByteMover(shared_ptr<path> filename, pos_int startPos)
		: filename_(filename), pos_(startPos)
	{ }
}
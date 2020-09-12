#pragma once
#include <cstddef>
#include <memory>
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using ::std::shared_ptr;

	struct IInsidePositionOwner
	{
		virtual void Addr(pos_int newPos)   = 0;
		virtual pos_int Addr() const        = 0;
		virtual ~IInsidePositionOwner()     = 0;
	};

	class File;
	template <typename T>
	shared_ptr<IInsidePositionOwner> MakePositionOwner(shared_ptr<File> filename, pos_int pos);
}

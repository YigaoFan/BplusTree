#pragma once
#include <cstddef>
#include <memory>
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using ::std::shared_ptr;
	using ::std::size_t;

	class InsidePositionOwner
	{
	private:
		pos_int _pos;
		size_t  _size;
	public:
		InsidePositionOwner();
		InsidePositionOwner(pos_int pos);
		InsidePositionOwner(pos_int pos, size_t size);
		void Addr(pos_int newPos);
		pos_int Addr() const;
		size_t Size() const;
		void Size(size_t size);
		virtual ~InsidePositionOwner() = 0;
	};

	class File;
	template <typename T>
	shared_ptr<InsidePositionOwner> MakePositionOwner(File* filename, pos_int pos);
}

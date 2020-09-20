#include "InsidePositionOwner.hpp"

namespace FuncLib::Store
{
	InsidePositionOwner::InsidePositionOwner() : InsidePositionOwner(0, 0)
	{ }

	InsidePositionOwner::InsidePositionOwner(pos_int pos) : InsidePositionOwner(pos, 0)
	{ }

	InsidePositionOwner::InsidePositionOwner(pos_int pos, size_t size) : _pos(pos), _size(size)
	{ }

	void InsidePositionOwner::Addr(pos_int newPos)
	{
		_pos = newPos;
	}

	pos_int InsidePositionOwner::Addr() const
	{
		return _pos;
	}

	size_t InsidePositionOwner::Size() const
	{
		return _size;
	}

	void InsidePositionOwner::Size(size_t size)
	{
		_size = size;
	}
}

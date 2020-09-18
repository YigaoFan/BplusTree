#pragma once
#include "NonTypeInsidePositionOwner.hpp"

namespace FuncLib::Store
{
	NonTypeInsidePositionOwner::NonTypeInsidePositionOwner(pos_int pos) : _pos(pos)
	{ }

	void NonTypeInsidePositionOwner::Addr(pos_int newPos)
	{
		_pos = newPos;
	}

	pos_int NonTypeInsidePositionOwner::Addr() const
	{
		return _pos;
	}
}

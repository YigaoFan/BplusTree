#pragma once
#include "InsidePositionOwner.hpp"

namespace FuncLib::Store
{
	class NonTypeInsidePositionOwner : public InsidePositionOwner
	{
	private:
		using Base = InsidePositionOwner;
	public:
		using Base::Base;
	};
}

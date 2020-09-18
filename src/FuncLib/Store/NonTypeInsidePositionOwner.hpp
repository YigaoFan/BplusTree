#pragma once
#include "IInsidePositionOwner.hpp"

namespace FuncLib::Store
{
	class NonTypeInsidePositionOwner : public IInsidePositionOwner
	{
	private:
		pos_int _pos;
	public:
		NonTypeInsidePositionOwner(pos_int pos);
		void Addr(pos_int newPos) override;
		pos_int Addr() const override;
	};
}

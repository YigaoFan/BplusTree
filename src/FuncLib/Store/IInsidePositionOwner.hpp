#include <cstddef>

namespace FuncLib::Store
{
	using ::std::size_t;

	using pos_int = size_t;

	struct IInsidePositionOwner
	{
		virtual void Addr(pos_int newPos)   = 0;
		virtual pos_int Addr() const        = 0;
		virtual size_t RequiredSize() const = 0;
		virtual ~IInsidePositionOwner()     = 0;
	};
}

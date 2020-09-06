#include <cstddef>

// TODO File 这些应该有个单独的子命名空间是不是
// 那也要建立子文件夹
namespace FuncLib::File
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

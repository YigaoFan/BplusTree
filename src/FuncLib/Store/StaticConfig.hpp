#include <cstddef>

namespace FuncLib::Store
{
	using ::std::size_t;

	constexpr size_t DiskBlockSize = 4096; // Depend on different OS setting
	using pos_int = size_t;
}

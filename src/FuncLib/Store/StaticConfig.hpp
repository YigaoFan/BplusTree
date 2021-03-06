#pragma once
#include <cstddef>
#include <climits>

namespace FuncLib::Store
{
	using ::std::size_t;

	constexpr size_t DiskBlockSize = 4096; // Depend on different OS setting
	using pos_int = size_t;
	using pos_label = int;
	constexpr pos_label FileLabel = 0;
	constexpr pos_label NonLabel = INT_MAX;
}

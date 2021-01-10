#pragma once
#include <cstddef>
#include <vector>

namespace FuncLib::Store::ObjectRelation
{
	using ::std::vector;

	template <typename T>
	static void Erase(size_t index, vector<T>& vec)
	{
		vec.erase(vec.begin() + index);
	}
}

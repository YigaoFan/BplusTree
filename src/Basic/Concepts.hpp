#pragma once
#include <type_traits>

namespace Basic
{
	template <typename T, typename Des>
	concept IsSameTo = std::is_same_v<T, Des>;

	template <typename T, typename Des>
	concept IsConvertibleTo = std::is_convertible_v<T, Des>;
}

#pragma once

namespace Collections {
	/**
 	* for container which pointer
 	*/
	template <typename Iter>
	auto
	ptrOff(Iter iter)
	{
		return *iter;
	}

	template <typename T>
	T
	copy(const T& t)
	{
		return t;
	}
}

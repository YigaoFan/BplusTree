#pragma once
/***********************************************************************************************************
   Basic type in Collections
***********************************************************************************************************/

#include <cstdint>
#include <functional>

namespace Collections
{
	typedef uint16_t order_int;
	typedef uint32_t key_int;
	template <typename Key>
	using LessThan = ::std::function<bool(Key const&, Key const&)>;
}
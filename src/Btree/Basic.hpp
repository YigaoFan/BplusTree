#pragma once
/***********************************************************************************************************
   Basic type in Collections
***********************************************************************************************************/

#include <cstdint>

namespace Collections
{
	typedef uint16_t order_int;
	typedef uint32_t key_int;
	template <typename Key>
	using LessThan = bool(Key const&, Key const&);
}
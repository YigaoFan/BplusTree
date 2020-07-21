#pragma once
/**********************************
   Debug
***********************************/
#include <string>
#include "Exception.hpp"

namespace Basic
{
	using ::std::string;
	using ::Basic::InvalidAccessException;

	void Assert(bool e, string message = "");
}

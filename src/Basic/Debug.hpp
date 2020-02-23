#pragma once
/**********************************
   Debug
***********************************/
#include <string>
#include "Exception.hpp"

namespace Debug
{
	using ::std::string;
	using ::Basic::InvalidAccessException;

	void Assert(bool e, string message = "");
}

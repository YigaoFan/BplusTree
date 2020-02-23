#include "Debug.hpp"

namespace Debug
{
	using ::std::string;
	using ::Basic::InvalidAccessException;

	void Assert(bool e, string message)
	{
		// TODO Add debug judge
		if (!e)
		{
			throw InvalidAccessException(message);
		}
	}
}
#include "Exception.hpp"

namespace Basic
{
	KeyNotFoundException::KeyNotFoundException()
		: runtime_error("Key not found in this container")
	{ }

	KeyNotFoundException::KeyNotFoundException(string message)
		: runtime_error(message + "Key not found in this container")
	{ }

	AccessOutOfRangeException::AccessOutOfRangeException()
		: runtime_error("Access out of the range")
	{ }

	InvalidAccessException::InvalidAccessException()
		: runtime_error("Invalid access")
	{ }

	InvalidAccessException::InvalidAccessException(string message)
		: runtime_error(message)
	{ }

	InvalidOperationException::InvalidOperationException()
		: runtime_error("Invalid operation")
	{ }

	InvalidOperationException::InvalidOperationException(string message)
		: runtime_error(message)
	{ }

	NotImplementException::NotImplementException()
		: runtime_error("Not implement")
	{ }

	NotImplementException::NotImplementException(string message)
		: runtime_error(message)
	{ }
}

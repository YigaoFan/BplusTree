#pragma once
#include <string>
#include <stdexcept>

namespace Exception
{
	using ::std::string;
	using ::std::runtime_error;
	using ::std::size_t;
	using ::std::move;

	class AssertionFailure : public runtime_error
	{
	public:
		AssertionFailure(string fileName, size_t line, string description, string expression)
			: runtime_error(fileName + ":" + ::std::to_string(line) + " " + description + "\n" + expression)
		{}
	};
}
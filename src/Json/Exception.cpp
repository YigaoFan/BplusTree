#include "Exception.hpp"

namespace Json
{
	InvalidStringException::InvalidStringException(LocationInfo info, string const& message)
		: runtime_error(string {&"InvalidString: " [ info.charAtLocation()]} // TODO [] is what grammar?
							+ " at " + to_string(info.Location) + " of ..." + info.charsAround() + ": " + message)
	{ }

	InvalidStringException::InvalidStringException(string const& message)
		: runtime_error(message)
	{ }

	ProgramError::ProgramError(string const& message)
		: runtime_error("This is a defense exception, please fix the error with message: " + message)
	{ }


	ParseEmptyStringException::ParseEmptyStringException()
		: runtime_error("Expect not empty string")
	{ }

	ParseExpectValueException::ParseExpectValueException()
		: runtime_error("Expect a string that not just contains space")
	{ }

	ParseNotSingleRootException::ParseNotSingleRootException(LocationInfo info)
		: runtime_error(string{&"JSON string is not a single root: " [ info.charAtLocation()]} // how this work
						+ " at " + to_string(info.Location) + " of ..." + info.charsAround()) // not worry, chars will be copied
	{ }

	PairNotFoundException::PairNotFoundException(size_t start, size_t end, char expected)
		: runtime_error(string{"Can't find "} + expected + " pair between " + to_string(start) + " and " + to_string(end))
	{ }

	ParseNumberTooBigException::ParseNumberTooBigException(const string& tooBigNum)
		: runtime_error(string{"Number: "} + tooBigNum + " is too big")
	{ }

	InvalidNumberException::InvalidNumberException(LocationInfo info)
		: runtime_error(string{"Char "} + info.charAtLocation() + " in " + info.charsAround() + " isn't meet expectation")
	{ }	
}
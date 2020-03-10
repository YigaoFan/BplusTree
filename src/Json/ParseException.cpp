#include "ParseException.hpp"

namespace Json
{
	InvalidStringException::InvalidStringException(LocationInfo info, string const& message)
		: runtime_error(string {&"InvalidString: " [ info.CharAtLocation()]} // TODO [] is what grammar?
							+ " at " + to_string(info.Location) + " of ..." + info.StringAround() + ": " + message)
	{ }

	InvalidStringException::InvalidStringException(string const& message)
		: runtime_error(message)
	{ }

	ProgramError::ProgramError(string const& message)
		: runtime_error("This is a defense exception, please fix the error with message: " + message)
	{ }

	ParseExpectValueException::ParseExpectValueException()
		: runtime_error("Expect a string that not just contains space")
	{ }

	ParseNotSingleRootException::ParseNotSingleRootException(LocationInfo info)
		: runtime_error(string{&"JSON string is not a single root: " [ info.CharAtLocation()]} // how this work
						+ " at " + to_string(info.Location) + " of ..." + info.StringAround()) // not worry, chars will be copied
	{ }

	ParseNumberTooBigException::ParseNumberTooBigException(string const& tooBigNum)
		: runtime_error(string{"Number: "} + tooBigNum + " is too big")
	{ }

	InvalidNumberException::InvalidNumberException(LocationInfo info)
		: runtime_error(string{"Char "} + info.CharAtLocation() + " in " + info.StringAround() + " isn't meet expectation")
	{ }	
}
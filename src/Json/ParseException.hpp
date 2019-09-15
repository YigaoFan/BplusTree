#pragma once
#include <exception>
#include "LocationInfo.hpp"

namespace Json {
	using std::runtime_error;
	using std::to_string;

	class WrongCharException {
	public:
		// TODO add detail info in constructor
		WrongCharException(char wrongChar)
		{

		}
	};

	class InvalidStringException : public runtime_error {
	public:
		InvalidStringException(LocationInfo info, string message="")
			: runtime_error("InvalidString: " + info.charAtLocation()
							+ " at " + to_string(info.Location) + " of ..." + info.charsAround());
		{ }
	};

	class WrongPairException {

	};

	class ParseEmptyStringException : public runtime_error {
	public:
		ParseEmptyStringException()
			: runtime_error("Expect not empty string")
		{ }
	};

	class ParseExpectValueException : public runtime_error {
	public:
		ParseExpectValueException()
			: runtime_error("Expect a string that not just contains space")
		{ }
	};

	class ParseNotSingleRootException : public runtime_error {
	public:
		ParseNotSingleRootException(LocationInfo info)
			: runtime_error("JSON string is not a single root: " + info.charAtLocation()
							+ " at " + to_string(info.Location) + " of ..." + info.charsAround()); // not worry, chars will be copied
		{ }
	};

	class PairNotFoundException : public runtime_error {
	public:
		PairNotFoundException(size_t start, size_t end, char expected)
			: runtime_error("Can't find " + expected + " pair between " + to_string(start) + " and " + to_string(end))
		{ }
	};
}
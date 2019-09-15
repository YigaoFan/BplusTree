#pragma once
#include <exception>
#include "LocationInfo.hpp"

namespace Json {
	using std::runtime_error;
	using std::to_string;

	class InvalidStringException : public runtime_error {
	public:
		explicit InvalidStringException(LocationInfo info, const string& message="")
			: runtime_error(string {&"InvalidString: " [ info.charAtLocation()]}
							+ " at " + to_string(info.Location) + " of ..." + info.charsAround() + ": " + message)
		{ }

		explicit InvalidStringException(const string& message)
			: runtime_error(message)
		{}
	};

	class ProgramError : public runtime_error {
	public:
		explicit ProgramError(const string& message)
			: runtime_error("This is a defense exception, please fix the error with message: " + message) // it will lead bug by use reference?
		{ }
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
		explicit ParseNotSingleRootException(LocationInfo info)
			: runtime_error(string{&"JSON string is not a single root: " [ info.charAtLocation()]} // how this work
							+ " at " + to_string(info.Location) + " of ..." + info.charsAround()) // not worry, chars will be copied
		{ }
	};

	class PairNotFoundException : public runtime_error {
	public:
		PairNotFoundException(size_t start, size_t end, char expected)
			: runtime_error(string{"Can't find "} + expected + " pair between " + to_string(start) + " and " + to_string(end))
		{ }
	};
}
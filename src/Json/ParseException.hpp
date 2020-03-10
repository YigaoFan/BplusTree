#pragma once
#include <stdexcept>
#include "LocationInfo.hpp"
/**********************************
   Exception in Json Parser
***********************************/

namespace Json 
{
	using ::std::runtime_error;
	using ::std::to_string;

	class InvalidStringException : public runtime_error
	{
	public:
		explicit InvalidStringException(LocationInfo info, string const& message="");
		explicit InvalidStringException(string const& message);
	};

	class ProgramError : public runtime_error
	{
	public:
		explicit ProgramError(string const& message);
	};

	class ParseExpectValueException : public runtime_error
	{
	public:
		ParseExpectValueException();
	};

	class ParseNotSingleRootException : public runtime_error
	{
	public:
		explicit ParseNotSingleRootException(LocationInfo info);
	};

	class ParseNumberTooBigException : public runtime_error
	{
	public:
		explicit ParseNumberTooBigException(string const& tooBigNum);
	};

	class InvalidNumberException : public runtime_error
	{
	public:
		explicit InvalidNumberException(LocationInfo info);
	};
}
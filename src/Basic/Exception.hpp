#pragma once
/**********************************
   Exception
***********************************/
#include <string>
#include <stdexcept>

namespace Basic
{
	using ::std::runtime_error;
	using ::std::move;
	using ::std::string;

	class KeyNotFoundException : public runtime_error
	{
	public:
		explicit KeyNotFoundException();
		explicit KeyNotFoundException(string message);
	};

	class AccessOutOfRangeException : public runtime_error
	{
	public:
		explicit AccessOutOfRangeException();
	};

	class InvalidAccessException : public runtime_error
	{
	public:
		explicit InvalidAccessException();
		explicit InvalidAccessException(string message);
	};

	class InvalidOperationException : public runtime_error
	{
	public:
		explicit InvalidOperationException();
		explicit InvalidOperationException(string message);
	};

	class NotImplementException : public runtime_error
	{
	public:
		explicit NotImplementException();
		explicit NotImplementException(string message);
	};
}

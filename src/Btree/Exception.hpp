#pragma once
/**********************************
   Exception in Collections
***********************************/
#include <string>
#include <stdexcept>

namespace Collections
{
	using ::std::runtime_error;
	using ::std::move;
	using ::std::string;

	template <typename T>
	class DuplicateKeyException : public runtime_error
	{
	public:
		T DupKey;

		explicit DuplicateKeyException(T dupKey, string message = "duplicate key")
			: runtime_error(message), DupKey(move(dupKey))
		{ }
	};

	class KeyNotFoundException : public runtime_error
	{
	public:
		explicit KeyNotFoundException()
			: runtime_error("Key not found in this container")
		{}
		
		explicit KeyNotFoundException(string message)
			: runtime_error(message + "Key not found in this container")
		{}
	};

	class AccessOutOfRangeException : public runtime_error
	{
	public:
		explicit AccessOutOfRangeException()
			: runtime_error("Access out of the range")
		{}
	};

	class InvalidAccessException : public runtime_error
	{
	public:
		explicit InvalidAccessException()
			: runtime_error("Invalid access")
		{}
	};

	class InvalidOperationException : public runtime_error
	{
	public:
		explicit InvalidOperationException()
			: runtime_error("Invalid operation")
		{}

		explicit InvalidOperationException(string message)
			: runtime_error(message)
		{}
	};

	class NotImplementException : public runtime_error
	{
	public:
		explicit NotImplementException()
			: runtime_error("Not implement")
		{}

		explicit NotImplementException(string message)
			: runtime_error(message)
		{}
	};
}

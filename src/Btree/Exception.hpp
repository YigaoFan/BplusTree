#pragma once
/**********************************
   Exception in Collections
***********************************/
#include <exception>

namespace Collections
{
	using ::std::runtime_error;
	using ::std::move;

	template <typename T>
	class DuplicateKeyException : public runtime_error
	{
	public:
		T DupKey;

		explicit DuplicateKeyException(T dupKey, const string& message)
			: runtime_error(message), DupKey(move(dupKey))
		{ }
	};

	class KeyNotFoundException : public runtime_error
	{
	public:
		explicit KeyNotFoundException()
			: runtime_error("Key not found in this container")
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
}

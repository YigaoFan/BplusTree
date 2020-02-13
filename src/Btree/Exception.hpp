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
}

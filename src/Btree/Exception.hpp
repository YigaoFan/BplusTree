#pragma once
/**********************************
   Exception in Collections
***********************************/
#include <exception>

namespace Collections {
	using ::std::runtime_error;
	using ::std::move;

	template <typename T>
	class DuplicateKeyException : public runtime_error {
	public:
		T DupKey;

		explicit DuplicateKeyException(T dupKey, const string& message)
			: runtime_error(message), DupKey(move(dupKey))
		{ }
	};
}

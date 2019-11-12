#pragma once
/**********************************
   Exception in Btree
***********************************/
#include <exception>

namespace Btree {
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

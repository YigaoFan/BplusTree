#pragma once
/**********************************
   Exception in Btree
***********************************/
#include <exception>

namespace Btree {
	using ::std::runtime_error;

	class InvalidInput : public runtime_error {
	public:
		explicit InvalidInput(const string& message)
			: runtime_error(message)
		{ }
	};
}

#include "Parser.hpp"

namespace Json
{
	using ::std::string;
	using ::std::make_shared;
	using ::std::size_t;
	using ::std::strtod;
	using ::std::string_view;
	using ::std::move;
	using ::Debug::Assert;

	JsonObject Parse(string_view jsonStr)
	{
		return Parser(jsonStr).DoParse();
	}
}
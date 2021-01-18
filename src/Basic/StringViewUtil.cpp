#include "StringViewUtil.hpp"
#include <cctype>

namespace Basic
{
	using ::std::find_if_not;
	using ::std::isspace;
	using ::std::string_view;

	string_view TrimStart(string_view s)
	{
		auto isSpace = [](char c) { return isspace(c); };
		return s.substr(find_if_not(s.begin(), s.end(), isSpace) - s.begin());
	}

	string_view TrimEnd(string_view s)
	{
		auto isSpace = [](char c) { return isspace(c); };
		return s.substr(0, find_if_not(s.rbegin(), s.rend(), isSpace).base() - s.begin());
	}
}

#pragma once
#include <string>
#include <string_view>

namespace Json
{
	using ::std::string;
	using ::std::string_view;

	struct LocationInfo
	{
	public:
		string_view AllString;
		size_t Location;
		/// TODO out of string_view range
		/// should ensure string exists in scope of usage
		LocationInfo(string_view str, size_t location) : AllString(str), Location(location)
		{ }

		char CharAtLocation() const
		{
			return AllString[Location];
		}

		string StringAround() const
		{
			return string(Location >= 2 ? AllString.substr(Location - 2, 2) : AllString.substr(Location, 2));
		}
	};
}
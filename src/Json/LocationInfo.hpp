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

		/**
		 * should ensure string exists in scope of usage
		 */
		LocationInfo(string_view str, size_t location)
			: AllString(str), Location(location)
		{ }

		char charAtLocation() const
		{
			return AllString[Location];
		}

		string charsAround() const
		{
			return Location >= 4 ? AllString.substr(Location - 4, 9) : AllString.substr(Location, 9);
		}
	};
}
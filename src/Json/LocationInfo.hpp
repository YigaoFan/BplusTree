#pragma once
#include <string>

namespace Json {
	struct LocationInfo {
	public:
		const string& AllString;
		size_t Location;

		/**
		 * should ensure string exists in scope of usage
		 */
		LocationInfo(const string& str, size_t location)
			: AllString(str), Location(location)
		{ }

		inline
		char
		charAtLocation() const
		{
			return AllString[Location];
		}

		inline
		string
		charsAround() const
		{
			return Location >= 4 ? AllString.substr(Location-4, 9) : AllString.substr(Location, 9);
		}
	};
}
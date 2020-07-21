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
		/// TODO out of string_view range should ensure string exists in scope of usage
		// It's a problem
		LocationInfo(string_view str, size_t location);
		char CharAtLocation() const;
		string StringAround() const;
	};
}
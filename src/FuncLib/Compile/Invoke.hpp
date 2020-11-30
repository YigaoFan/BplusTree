#pragma once
#include <vector>
#include "../../Json/Json.hpp"

namespace FuncLib::Compile
{
	using Json::JsonObject;
	using ::std::vector;

	/// Invoke on *nix OS
	JsonObject Invoke(vector<char> const& bin, char const* name, JsonObject args);
	// TODO Invoke on Windows
}

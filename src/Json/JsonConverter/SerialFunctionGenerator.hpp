#pragma once
#include <vector>
#include <string>
#include <string_view>

namespace Json::JsonConverter
{
	using ::std::vector;
	using ::std::string;
	using ::std::string_view;

	vector<string> GenerateStructSerializer(vector<string_view> structDef);
}
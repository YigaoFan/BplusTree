#pragma once
#include <vector>
#include <string>
#include <string_view>

namespace Json::JsonConverter
{
	using ::std::vector;
	using ::std::string;
	using ::std::string_view;

	/*constexpr unsigned int StrToInt(char const* str, int h = 0)
	{
		return !str[h] ? 5381 : (StrToInt(str, h + 1) * 33) ^ str[h];
	}*/

	vector<string> GenerateStructDeserializer(vector<string_view> structDef);
}
#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <cctype>
#include "StructObject.hpp"
#include "WordEnumerator.hpp"

namespace Json::JsonConverter
{
	using ::std::string;
	using ::std::string_view;
	using ::std::vector;
	
	// Below item should be tested
	vector<string_view> Split(string_view str, char separator);
	bool Contain(string_view str, string_view des);
	string_view TrimEnd(string_view str);
	string_view TrimStart(string_view str);
	bool IsBlank(string_view str);

	string_view ParseTypeName(WordEnumerator& wordEnumerator);

	// Not support derive
	// Not support private data member
	// Not support default set value
	// Not support public keyword inside
	// Not support ref data member
	// 换行符处理过的
	StructObject ParseStruct(vector<string_view> structDef);
}
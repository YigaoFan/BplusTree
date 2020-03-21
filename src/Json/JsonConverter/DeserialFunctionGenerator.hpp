#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "StructParser.hpp"
#include "../Json.hpp"

namespace Json::JsonConverter
{
	using ::std::vector;
	using ::std::string;
	using ::std::string_view;

	constexpr unsigned int StrToInt(char const* str)
	{
		// how to implement
		// https://stackoverflow.com/questions/16388510/evaluate-a-string-with-a-switch-in-c
		return str + 1;
	}

	unsigned int RuntimeStrToInt(char const* str)
	{
		// how to implement
		// https://stackoverflow.com/questions/16388510/evaluate-a-string-with-a-switch-in-c
		return 1;
	}

	JsonType GetJsonType(string const& type)
	{
		switch (RuntimeStrToInt(type.c_str()))
		{
		case StrToInt("int"):
		case StrToInt("double"):
			return JsonType::Number;

		case StrToInt("string"):
			return JsonType::String;

		case StrToInt("bool"):
			return JsonType::True;

		case StrToInt("array"):
		case StrToInt("vector"):
			return JsonType::Array;

			// case null? is what?
		default:
			return JsonType::Object;
		}
	}

	vector<string> GenerateDeserializeFunction(vector<string_view> structDef)
	{
		// TODO 处理换行符，要不要处理，可以用类似 PipelineEnumerator 来做，那就需要重构下 WordEnumerator
		auto def = ParseStruct(structDef);
		vector<string> functionDef;

		for (auto& p : def.DataMembers())
		{
			auto& type = p.first;
			auto t = GetJsonType(type);
			auto& name = p.second;
			string line = "obj." + name + " = json[" + name + "].";
			switch (t)
			{
			case JsonType::Array:
				// Array element should be same type, each item should be convert, too
			case JsonType::Object:
			case JsonType::Number:
			case JsonType::String:
			case JsonType::True:
			case JsonType::False:
			case JsonType::Null:
			default:
				break;
			}

			functionDef.push_back(line);
		}

		return functionDef;
	}
}
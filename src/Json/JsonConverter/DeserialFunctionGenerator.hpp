#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "StructParser.hpp"
#include "../Json.hpp"
#include "../../Basic/Debug.hpp"

namespace Json::JsonConverter
{
	using ::std::vector;
	using ::std::string;
	using ::std::string_view;
	using ::Debug::Assert; // 这个在 Basic 目录下的命名空间居然是 Debug，不统一...

	constexpr unsigned int StrToInt(char const* str, int h = 0)
	{
		return !str[h] ? 5381 : (StrToInt(str, h + 1) * 33) ^ str[h];
	}

	JsonType GetJsonType(string_view type)
	{
		switch (StrToInt(type.data()))
		{
		case StrToInt("int"):
		case StrToInt("double"):
			return JsonType::Number; // 如果类型不一致，会自动有类型转换吗，还是需要手动 static_cast
		case StrToInt("string"):
			return JsonType::String;
		case StrToInt("bool"):
			return JsonType::True; // TODO should be bool
		case StrToInt("array"):
		case StrToInt("vector"):
			return JsonType::Array;
			// case null? is what? C++ 没有这个类型表示
		default:
			return JsonType::Object;
		}
	}

	vector<string> GenerateStructDeserializer(vector<string_view> structDef)
	{
		// TODO 处理换行符，要不要处理，可以用类似 PipelineEnumerator 来做，那就需要重构下 WordEnumerator
		auto def = ParseStruct(structDef);
		vector<string> functionDef;
		auto type = def.TypeName();
		functionDef.push_back("template <>");
		functionDef.push_back(type + " Deserialize<" + type + ">(JsonObject const& json)");
		functionDef.push_back("{");
		functionDef.push_back("return");
		functionDef.push_back("{");

		for (auto& p : def.DataMembers())
		{
			auto& type = p.first;
			auto& name = p.second;
			string line = ("Deserialize<" + type + ">(json[\"" + name + "\"]),");
			functionDef.push_back(line);
		}

		functionDef.push_back("};");
		functionDef.push_back("}");

		return functionDef;
	}
}
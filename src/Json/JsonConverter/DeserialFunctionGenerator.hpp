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
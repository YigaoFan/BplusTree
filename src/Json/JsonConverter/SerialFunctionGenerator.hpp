#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "StructParser.hpp"

namespace Json::JsonConverter
{
	using ::std::vector;
	using ::std::string;
	using ::std::string_view;

	vector<string> GenerateStructSerializer(vector<string_view> structDef)
	{
		auto def = ParseStruct(structDef);
		vector<string> functionDef;
		auto type = def.TypeName();
		functionDef.push_back("template <>");
		functionDef.push_back("string Serialize<" + type + ">(" + type +" const& t)");
		functionDef.push_back("{");
		functionDef.push_back("string str;");

		for (auto& p : def.DataMembers())
		{
			auto& type = p.first;
			auto& name = p.second;
			string line = ("str += Serialize<" + type + ">(t." + name + ");");
			functionDef.push_back(line);
		}

		functionDef.push_back("return str;");
		functionDef.push_back("}");

		return functionDef;
	}
}
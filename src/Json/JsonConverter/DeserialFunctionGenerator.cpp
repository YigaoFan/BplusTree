#include "DeserialFunctionGenerator.hpp"
#include "StructParser.hpp"

namespace Json::JsonConverter
{
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
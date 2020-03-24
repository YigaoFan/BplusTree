#include "DeserialFunctionGenerator.hpp"
#include "StructParser.hpp"

namespace Json::JsonConverter
{
	vector<string> GenerateStructDeserializer(vector<string_view> structDef)
	{
		// TODO �����з���Ҫ��Ҫ�������������� PipelineEnumerator �������Ǿ���Ҫ�ع��� WordEnumerator
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
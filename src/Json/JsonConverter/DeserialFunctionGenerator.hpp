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
	using ::Debug::Assert; // ����� Basic Ŀ¼�µ������ռ��Ȼ�� Debug����ͳһ...

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
			return JsonType::Number; // ������Ͳ�һ�£����Զ�������ת���𣬻�����Ҫ�ֶ� static_cast
		case StrToInt("string"):
			return JsonType::String;
		case StrToInt("bool"):
			return JsonType::True; // TODO should be bool
		case StrToInt("array"):
		case StrToInt("vector"):
			return JsonType::Array;
			// case null? is what? C++ û��������ͱ�ʾ
		default:
			return JsonType::Object;
		}
	}

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
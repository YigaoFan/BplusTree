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

	vector<string> GenerateDeserializerForStruct(vector<string_view> structDef)
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
			auto t = GetJsonType(type);
			string line;
			line += "Deserialize<";
			switch (t)
			{
			case JsonType::Array:
				// Array element should be same type (not explict check), each item should be convert, too
				// should distinguish vector and array
				// if vector, should know Item type
				// if array, should know Item type and Count
				// should use help of structure binding?
				// ������ôʹ�� Struct �� type ���Ի��һЩ�����ͣ������� vector �� T
				/*line += "vector";
				line += "<T>";*/
				line += type;

				break;
			case JsonType::Object:
				line += type;
				break;

			case JsonType::Number:
				line += "int";// int or double depend on struct def
				break;
			case JsonType::String:
				line += "string";
				break;
			case JsonType::True:
			case JsonType::False:
				line += "bool";
				break;
			case JsonType::Null:
				throw;// TODO add exception
			default:
				break;
			}

			line += '>';
			line += "(json[\"" + name + "\"]),";
			functionDef.push_back(line);
		}

		functionDef.push_back("};");
		functionDef.push_back("}");

		return functionDef;
	}
}
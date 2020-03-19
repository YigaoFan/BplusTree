#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <cctype>
#include "StructObject.hpp"

namespace Json::JsonConverter
{
	using ::std::string;
	using ::std::string_view;
	using ::std::vector;
	
	// Below item should be tested

	vector<string_view> Split(string_view str, char delimiter)
	{
		vector<string_view> splited;

		for (auto i = 0, last = 0; i < str.size(); ++i)
		{
			if (str[i] == delimiter)
			{
				splited.emplace_back(str.substr(last, i - last));
				last = i + 1;
			}
		}

		return splited;
	}

	bool Contain(string_view str, string_view des)
	{
		return str.find(des) != string_view::npos;
	}

	string_view TrimEnd(string_view str)
	{
		using ::std::isblank;
		
		for (auto size = str.size(); size >= 0; --size)
		{
			if (!isblank(static_cast<unsigned char>(str[size - 1])))
			{
				return str.substr(0, size);
			}
		}

		return str.substr(0, 0);
	}

	string_view TrimStart(string_view str)
	{
		using ::std::isblank;

		for (auto i = 0; i < str.size(); ++i)
		{
			if (!isblank(static_cast<unsigned char>(str[i])))
			{
				return str.substr(i, str.size() - i);
			}
		}

		return str.substr(0, 0);
	}

	bool IsBlank(string_view str)
	{
		for (auto c : str)
		{
			if (!isblank(static_cast<unsigned char>(c)))
			{
				return false;
			}
		}

		return true;
	}

	string_view ParseTypeName(vector<string>& structDef, uint32_t& i)
	{
		string_view structKeyword = "struct";
		while (!Contain(structDef[i], structKeyword))
		{
			++i;
		}

		// Should have something like Enumerator or generator to unify line internal and between lines
		auto structLine = structDef[i];
		auto nextWordIndex = structLine.find(structKeyword) + structKeyword.size();
		if (nextWordIndex < structLine.size())
		{
			auto sub = structLine.substr(nextWordIndex, structLine.size() - nextWordIndex);
			if (!IsBlank(sub))
			{
				return TrimEnd(TrimStart(sub));
			}
			else
			{
				++i;
				while (!IsBlank(structDef[i]))
				{
					auto sub = structDef[i];
					return TrimEnd(TrimStart(sub));
				}

				throw;// TODO Exception
			}
		}
	}

	// Not support derive
	// Not support private data member
	// Not support default set value
	// Not support public keyword inside
	StructObject ParseStruct(vector<string> structDef)
	{
		uint32_t i = 0;
		
		auto obj = StructObject(ParseTypeName(structDef, i));

		auto tokens = Split(structDef.substr(0, i), ' ');// Enter
		auto structName = tokens[1];
		

		auto j = ++i;
		while (structDef[j] != '}')
		{
			++i;
		}
		// Remove all blank between ; in {}
		auto datas = Split(structDef.substr(i, j - 1), ';');

		for (auto& d : datas)
		{
			auto infos = Split(d, ' ');
			obj.AppendDataMember(infos[0], infos[1]);
		}
	}
}
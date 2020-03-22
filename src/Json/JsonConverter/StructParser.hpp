#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <cctype>
#include "StructObject.hpp"
#include "../ParseException.hpp"
#include "../../Basic/Debug.hpp"
#include "WordEnumerator.hpp"

namespace Json::JsonConverter
{
	using ::std::string;
	using ::std::string_view;
	using ::std::vector;
	using ::Debug::Assert;
	
	// Below item should be tested

	vector<string_view> Split(string_view str, char separator)
	{
		vector<string_view> splited;
		auto i = 0, unitStart = 0;
		auto separated = true;

		auto tryAdd = [&splited](auto str, auto start, auto index)
		{
			if (start != index)
			{
				splited.emplace_back(str.substr(start, index - start));
			}
		};
		// " abc "
		// "  abc "
		// "abc "
		// " abc"
		for (; i < str.size(); ++i)
		{
			if (str[i] == separator)
			{
				tryAdd(str, unitStart, i);
				unitStart = i + 1;
			}
		}

		tryAdd(str, unitStart, i);
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
				return str.substr(i);
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

	string_view ParseTypeName(WordEnumerator& wordEnumerator)
	{
		auto& en = wordEnumerator;
		string_view structKeyword = "struct";
		while (en.MoveNext())
		{
			if (en.Current() == structKeyword)
			{
				goto GetTypeName;
			}
		}

		goto NotFound;

	GetTypeName:
		if (en.MoveNext())
		{
			return en.Current();
		}

	NotFound:
		throw InvalidStringException("Type name of struct not found");
	}

	// Not support derive
	// Not support private data member
	// Not support default set value
	// Not support public keyword inside
	// Not support ref data member
	// 换行符处理过的
	StructObject ParseStruct(vector<string_view> structDef)
	{
		// Remove the newline in string_view
		auto e = WordEnumerator(structDef, ' ');
		auto def = StructObject(ParseTypeName(e));

		if (e.MoveNext() && e.Current() == "{")
		{
			e.ChangeSeparator(';');
			while (e.MoveNext() && !Contain(e.Current(), "}"))
			{
				auto var = TrimEnd(TrimStart(e.Current()));
				auto infos = Split(var, ' ');
				Assert(infos.size() == 2, "data member not split into two part");
				def.AppendDataMember(infos[0], infos[1]);
			}

			return def;
		}
		else
		{
			// 按理说，这些语法方面应该由编译器去检查
			throw InvalidStringException("struct doesn't have {");
		}
	}
}
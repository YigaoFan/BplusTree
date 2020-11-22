#pragma once
#include <string>
#include <vector>
#include <string_view>
#include <cstdlib>
#include <iostream>
#include "FuncObj.hpp"
#include "../../Basic/Exception.hpp"
#include "ParseFunc.hpp"
#include "FuncDefTokenReader.hpp"
#include "../../Json/JsonConverter/WordEnumerator.hpp"

namespace FuncLib::Compile
{
	using Basic::InvalidOperationException;
	using Json::JsonConverter::WordEnumerator;
	using ::std::string;
	using ::std::string_view;
	using ::std::to_string;
	using ::std::vector;

	void CompileOnUnix()
	{
		// system("");
	}

	void CompileOnWindows()
	{
		// system("");
	}

	void CheckGrammar(FuncDefTokenReader* defReader)
	{

	}

	// add extern
	// get function def
	// get function type
	// 改名，因为 extern 不支持重载
	// scan | compile | addToLib | addToIndex
	void CheckProhibitedUseage(vector<string> const& funcBody)
	{
		for (auto& l : funcBody)
		{
			// 这里的检测还比较粗，因为下面这样的 new 有可能在一个变量名末尾或者注释中
			if (l.find("new ") != string::npos)
			{
				throw InvalidOperationException("Not permit to use new operator in lib function");
			}
		}
	}

	char const* FuncNameSuffix = "_impl";

	pair<array<string, 3>, vector<string>> GenerateWrapperFunc(pair<array<string, 3>, vector<string>> const& funInfo)
	{
		auto [returnType, name, args] = funInfo.first;
		array<string, 3> typeInfos
		{
			"JsonObject",
			name,
			"JsonObject jsonObj",
		};

		auto divideArg = [](string_view s, char delimiter)
		{
			auto i = s.find_first_of(delimiter);
			return array<string_view, 2>
			{
				s.substr(0, i), 
				s.substr(i)
			};
		};

		WordEnumerator e{ vector<string_view>{ args }, ',' };
		vector<string> argDeserialCodes;

		// Deserialize code
		while (e.MoveNext())
		{
			if (not e.Current().empty())
			{
				auto [type, name] = divideArg(e.Current(), ' ');
				argDeserialCodes.push_back(string("JsonConverter::Deserialize<") + type.data() + ">(jsonObj)");
			}
		}

		auto ConsArgTupleStr = [](vector<string> initCodes)
		{
			string tupleStr("tuple argsTuple{");
			for (auto& a : initCodes)
			{
				tupleStr.append(a);
			}
			tupleStr.append("};");
			return tupleStr;
		};
		// tuple 无参数也行
		// 改名字
		string argsTuple = ConsArgTupleStr(move(argDeserialCodes));

		// add JsonConverter, std::apply header(<tuple> file)
		string invokeStatement("auto r = apply(" + name + FuncNameSuffix + ", move(argsTuple));");

		string returnStatement("return JsonConverter::Serialize(r);");
		vector<string> body
		{
			argsTuple,
			invokeStatement,
			returnStatement,
		};
		return 
		{
			move(typeInfos),
			move(body)
		};
	}

	/// include 所有必要的头文件，让外界方便使用，后期需求
	FuncObj Compile(FuncDefTokenReader* defReader)
	{
		CheckGrammar(defReader);

		auto funcs = ParseFunc(defReader);
		decltype(funcs) cookedFuncs;

		for (auto& f : funcs)
		{
			auto& t = f.first;
			std::cout << "Parse type result: \"" << t[0] << "\" \"" << t[1] << "\" \"" << t[2] << "\"" << std::endl;
			auto& b = f.second;

			std::cout << "Parse body result: " << std::endl;
			for (auto& l : b)
			{
				std::cout << l << std::endl;
			}

			CheckProhibitedUseage(f.second);
			cookedFuncs.push_back(GenerateWrapperFunc(f));
			f.first[1] = f.first[1] + FuncNameSuffix;

			std::cout << "end" << std::endl;
		}

#ifdef _MSVC_LANG
		CompileOnWindows();
#else // __clang__ or __GNUC__
		CompileOnUnix();
#endif

		// 先用编译器编译一遍，有报错直接停止
		// get func type
		// convert to JsonObject arg type and return type
		// call compiler to compile it，如果有编译错误怎么办？
		// read compiled file into memory

		return {};
	}
}
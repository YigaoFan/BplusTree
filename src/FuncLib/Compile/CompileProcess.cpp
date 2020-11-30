
#include <string>
#include <string_view>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "CompileProcess.hpp"
#include "../../Basic/Exception.hpp"
#include "../../Btree/Generator.hpp"
#include "ParseFunc.hpp"

namespace FuncLib::Compile
{
	using Basic::InvalidOperationException;
	using Basic::NotImplementException;
	using Collections::RecursiveGenerator;
	using ::std::ifstream;
	using ::std::move;
	using ::std::ofstream;
	using ::std::string;
	using ::std::string_view;
	using ::std::to_string;
	using ::std::vector;

	struct ExternCBody
	{
		RecursiveGenerator<string> ToLineCode()
		{
			co_yield {};
		}
	};

	struct AppendCode
	{
		vector<string> IncludeNames;
		ExternCBody ExternCBody;

		RecursiveGenerator<string> ToLineCode()
		{
			for (auto& i :IncludeNames)
			{
				co_yield "#include \"" + i + "\"";
			}

			co_yield "extern \"C\"";
			co_yield "{";
			co_yield ExternCBody.ToLineCode();
			co_yield "}";
		}
	};

	/// 返回编译过后的字节
	// read compiled file into memory
	vector<char> CompileOnUnix(FuncDefTokenReader* defReader, AppendCode* appendCode)
	{
		char const* filename = "temp_compile";
		ofstream f(filename);
		// read defReader TODO
		auto g = appendCode->ToLineCode();// 记得把这句移出这个函数
		while (g.MoveNext())
		{
			f << g.Current();
		}

		auto compileCmd = string("g++ -m64 -g -shared-libgcc -fPIC -o ") + filename + ".so " + filename + ".cpp -std=c++0x -shared";
		system(compileCmd.c_str());

		// 删掉相关的临时文件
		return {};
	}

	vector<char> CompileOnWindows(FuncDefTokenReader* defReader, AppendCode* appendCode)
	{
		throw NotImplementException("CompileOnWindows function not implemented");
	}

	/// Has ResetReadPos effect
	void CheckGrammar(FuncDefTokenReader* defReader)
	{
		defReader->ResetReadPos();
		char const *filename = "temp_compile";

		auto compileCmd = string("g++ -m64 -g -shared-libgcc -fPIC -o ") + filename + ".so " + filename + ".cpp -std=c++0x -shared";
		if (system(compileCmd.c_str()) != 0)
		{
			throw InvalidOperationException("function definitions have compile error");
		}

		defReader->ResetReadPos();
	}

	// add extern，extern 内只需要有 wrapper 函数
	// get function def
	// get function type
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

	vector<string> GenerateWrapperFunc(FuncType const& funcType)
	{
		auto& returnType = funcType.ReturnType();
		auto& name = funcType.FuncName();
		auto& argTypes = funcType.ArgTypes();
		vector<string> wrapperFuncDef
		{
			"JsonObject " + name + "_wrapper(JsonObject jsonObj)",
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

		vector<string> argDeserialCodes;
		// Deserialize code
		for (auto& t : argTypes)// names 可能有用
		{
			auto name = "";// TODO
			argDeserialCodes.push_back(string("JsonConverter::Deserialize<") + t + ">(jsonObj[" + name + "])");
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

		string argsTuple = ConsArgTupleStr(move(argDeserialCodes));

		// add JsonConverter, std::apply header(<tuple> file)
		string invokeStatement("auto r = std::apply(" + name + ", std::move(argsTuple));");

		string returnStatement("return JsonConverter::Serialize(r);");
		wrapperFuncDef.push_back(move(argsTuple));
		wrapperFuncDef.push_back(move(invokeStatement));
		wrapperFuncDef.push_back(move(returnStatement));

		return wrapperFuncDef;
	}
	
	pair<vector<FuncObj>, vector<char>> Compile(FuncDefTokenReader* defReader)
	{
		CheckGrammar(defReader);

		auto funcs = ParseFunc(defReader);
		vector<vector<string>> wrapperFuncsDef;
		vector<FuncObj> funcObjs;

		for (auto& f : funcs)
		{
			auto& type = f.first;
			// std::cout << "Parse type result: \"" << t[0] << "\" \"" << t[1] << "\" \"" << t[2] << "\"" << std::endl;
			auto& b = f.second;

			// std::cout << "Parse body result: " << std::endl;
			// for (auto& l : b)
			// {
			// 	std::cout << l << std::endl;
			// }

			// 这样的话，f.second 即 body 根本没必要返回
			CheckProhibitedUseage(f.second);
			wrapperFuncsDef.push_back(GenerateWrapperFunc(type));
			string summary;
			funcObjs.push_back(FuncObj{ move(type), summary });

			// std::cout << "end" << std::endl;
		}

		defReader->ResetReadPos();
		AppendCode appendCode;
		appendCode.IncludeNames.push_back("Json.hpp");
		appendCode.IncludeNames.push_back("tuple");
#ifdef _MSVC_LANG
		auto bins = CompileOnWindows(defReader, &appendCode);
#else // __clang__ or __GNUC__
		auto bins = CompileOnUnix(defReader, &appendCode);
#endif

		// 先用编译器编译一遍，有报错直接停止
		// get func type，类型信息可以当作 summary 辅助外面调用
		// convert to JsonObject arg type and return type
		// call compiler to compile it，如果有编译错误怎么办？

		return { move(funcObjs), bins };
	}
}
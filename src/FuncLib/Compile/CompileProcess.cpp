
#include <array>
#include <string>
#include <vector>
#include <string_view>
#include <cstdlib>
#include <fstream>
#include "../Basic/Exception.hpp"
#include "CompileProcess.hpp"
#include "AppendCode.hpp"
#include "ParseFunc.hpp"
#include "Util.hpp"

namespace FuncLib::Compile
{
	using Basic::InvalidOperationException;
	using Basic::NotImplementException;
	using ::std::array;
	using ::std::get;
	using ::std::move;
	using ::std::ofstream;
	using ::std::string;
	using ::std::string_view;
	using ::std::to_string;
	using ::std::vector;
	
	vector<char> CompileOnWindows(FuncDefTokenReader* defReader, AppendCode* appendCode)
	{
		throw NotImplementException("CompileOnWindows function not implemented");
	}

	template <typename GeneratorPtr, typename... GeneratorPtrs>
	void AppendCodeTo(ofstream* file, GeneratorPtr generatorPtr, GeneratorPtrs... generatorPtrs)
	{
		while (generatorPtr->MoveNext())
		{
			(*file) << generatorPtr->Current() << std::endl;
		}

		if constexpr (sizeof...(GeneratorPtrs) > 0)
		{
			AppendCodeTo(file, generatorPtrs...);
		}
	}

	template <typename... Generators>
	auto DoCompile(auto afterCompileCallback, Generators... codeContentGenerators)
	{
		FilesCleaner cleaner;

		string name = "temp_compile_" + RandomString(); // 加入随机性
		string cppFileName = name + ".cpp";
		ofstream f(cppFileName);
		// cleaner.Add(cppFileName); TODO 恢复这行

		AppendCodeTo(&f, (&codeContentGenerators)...);

		string soFileName = name + ".so";
		auto compileCmd = string("g++ -shared -fPIC -o ") + soFileName + " " + cppFileName + " -std=c++2a";
		auto r = system(compileCmd.c_str());
		cleaner.Add(soFileName);

		return afterCompileCallback(cppFileName, soFileName, r);
	}

	/// 返回编译过后的字节
	vector<char> GetCompiledByteOnUnix(FuncDefTokenReader* defReader, AppendCode const* appendCode)
	{
		defReader->ResetReadPos();
		auto g1 = defReader->GetLineCodeGenerator();
		auto g2 = appendCode->GetLineCodeGenerator();
		auto g = defReader->GetLineCodeGenerator();
		return DoCompile([](auto cppFileName, auto soFileName, auto cmdReturnValue)
		{
			if (cmdReturnValue != 0)
			{
				throw InvalidOperationException("function definitions have compile error");
			}

			return ReadFileBytes(soFileName.c_str());
		}, move(g1), move(g2));
	}

	/// Will ResetReadPos
	FuncDefTokenReader& CheckGrammar(FuncDefTokenReader& defReader)
	{
		defReader.ResetReadPos();
		auto g = defReader.GetLineCodeGenerator();
		DoCompile([](auto cppFileName, auto soFileName, auto cmdReturnValue)
		{
			if (cmdReturnValue != 0)
			{
				throw InvalidOperationException("function definitions have compile error");
			}
		}, move(g));

		return defReader;
	}

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

	vector<string> GenerateWrapperFunc(FuncType const& funcType, vector<string> const& paraNames)
	{
		auto& returnType = funcType.ReturnType;
		auto& name = funcType.FuncName;
		auto& paraTypes = funcType.ArgTypes;
		vector<string> wrapperFuncDef
		{
			"Json::JsonObject " + name + "_wrapper(Json::JsonObject jsonObj) {",
		};

		vector<string> argDeserialCodes;
		// Deserialize code
		for (auto i = 0; i < paraTypes.size(); ++i)
		{
			auto& t = paraTypes[i];
			auto& n = paraNames[i];
			argDeserialCodes.push_back(string("Json::JsonConverter::Deserialize<") + t + ">(jsonObj[\"" + n + "\"])");
		}

		auto ConsArgTupleStr = [](vector<string> initCodes)
		{
			// tuple 无参数也行
			string tupleStr("std::tuple argsTuple{");
			for (auto& a : initCodes)
			{
				tupleStr.append(a + ", ");
			}
			tupleStr.append("};");
			return tupleStr;
		};

		string argsTuple = ConsArgTupleStr(move(argDeserialCodes));

		// add JsonConverter, std::apply header(<tuple> file)
		string invokeStatement, returnStatement;
		if (returnType == "void")
		{
			invokeStatement = string("std::apply(" + name + ", std::move(argsTuple));");
			returnStatement = string("return Json();");
		}
		else
		{
			invokeStatement = string("auto r = std::apply(" + name + ", std::move(argsTuple));");
			returnStatement = string("return Json::JsonConverter::Serialize(r);");
		}

		wrapperFuncDef.push_back(move(argsTuple));
		wrapperFuncDef.push_back(move(invokeStatement));
		wrapperFuncDef.push_back(move(returnStatement));
		wrapperFuncDef.push_back("}");

		return wrapperFuncDef;
	}

	pair<vector<vector<string>>, vector<FuncObj>> ProcessFuncs(vector<tuple<FuncType, vector<string>, vector<string>>> funcs)
	{
		vector<vector<string>> wrapperFuncsDef;
		vector<FuncObj> funcObjs;

		for (auto& f : funcs)
		{
			auto& type = get<0>(f);
			auto& paraNames = get<1>(f);
			auto& body = get<2>(f);

			CheckProhibitedUseage(body);
			wrapperFuncsDef.push_back(GenerateWrapperFunc(type, paraNames));
			funcObjs.push_back(FuncObj{move(type), move(paraNames), {}});
		}

		return pair(move(wrapperFuncsDef), move(funcObjs));
	};

	// 不太清楚 ParseFunc 在这个 operator 的返回值 decltype(auto) 推导出来是不是引用类型，
	// 是引用类型的话，应该是返回一个局部变量的，那就有问题了，但是编译器没有报 warning
	// 看了下编辑器鼠标放上去的提示，不是引用类型，之后补一下相关的知识
	decltype(auto) operator| (FuncDefTokenReader& reader, auto processor)
	{
		return processor(reader);
	}

	auto operator| (vector<tuple<FuncType, vector<string>, vector<string>>> funcs, auto processor)
	{
		return processor(move(funcs));
	}

	// TODO extern C 里返回 Json 会warning，看 StackOverflow 上说是防止 C 程序使用不正确，那调用程序是 C++ 程序呢，测试一下
	pair<vector<FuncObj>, vector<char>> Compile(FuncDefTokenReader defReader)
	{
		auto [wrapperFuncsDef, funcObjs] = defReader | ParseFunc | ProcessFuncs;
		
		AppendCode code;
		code.IncludeNames.push_back("../src/Json/JsonConverter/JsonConverter.hpp");
		code.IncludeNames.push_back("tuple");
		code.ExternCBody.WrapperFuncDefs = move(wrapperFuncsDef);

		defReader.ResetReadPos();
#ifdef _MSVC_LANG
		auto bins = CompileOnWindows(defReader, &appendCode);
#else // __clang__ or __GNUC__
		auto bins = GetCompiledByteOnUnix(&defReader, &code);
#endif

		return { move(funcObjs), bins };
	}
}
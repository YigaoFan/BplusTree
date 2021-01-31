
#include <string>
#include <vector>
#include <fstream>
#include <string_view>
#include "../Basic/Exception.hpp"
#include "CompileProcess.hpp"
#include "AppendCode.hpp"
#include "ParseFunc.hpp"
#include "Util.hpp"

namespace FuncLib::Compile
{
	using Basic::InvalidOperationException;
	using Basic::NotImplementException;
	using ::std::get;
	using ::std::move;
	using ::std::ofstream;
	using ::std::string;
	using ::std::string_view;
	using ::std::to_string;
	using ::std::vector;
	
	vector<char> CompileOnWindows(FuncsDefReader* defReader, AppendCode* appendCode)
	{
		throw NotImplementException("CompileOnWindows function not implemented");
	}

	template <typename GeneratorPtr, typename... GeneratorPtrs>
	void AppendCodeTo(ofstream* file, GeneratorPtr generatorPtr, GeneratorPtrs... generatorPtrs)
	{
		while (generatorPtr->MoveNext())
		{
			(*file) << generatorPtr->Current();
		}

		if constexpr (sizeof...(GeneratorPtrs) > 0)
		{
			AppendCodeTo(file, generatorPtrs...);
		}
		else
		{
			file->flush();
		}
	}

	template <typename... Generators>
	auto DoCompile(auto afterCompileCallback, Generators... codeContentGenerators)
	{
		FilesCleaner cleaner;

		string name = "temp_compile_" + RandomString(); // 加入随机性
		string cppFileName = name + ".cpp";
		ofstream f(cppFileName);
		cleaner.Add(cppFileName);

		AppendCodeTo(&f, (&codeContentGenerators)...);

		string soFileName = name + ".so";
		auto compileCmd = string("g++ -O2 -shared -fPIC -I../src -o ") + soFileName + " " + cppFileName + " -std=c++2a"; // TODO > 重定向编译输出
		auto r = system(compileCmd.c_str());
		cleaner.Add(soFileName);

		return afterCompileCallback(cppFileName, soFileName, r);
	}

	/// 返回编译过后的字节
	vector<char> GetCompiledByteOnUnix(FuncsDefReader* defReader, AppendCode const* appendCode)
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

	FuncsDefReader& CheckGrammar(FuncsDefReader& defReader)
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

	vector<string> GenerateWrapperFunc(FuncType const& funcType, vector<string> const& paraNames)
	{
		auto& returnType = funcType.ReturnType;
		auto& name = funcType.FuncName;
		auto& paraTypes = funcType.ArgTypes;
		vector<string> wrapperFuncDef
		{
			"Json::JsonObject " + GetWrapperFuncName(name) + "(Json::JsonObject jsonObj) {",
		};

		vector<string> argDeserialCodes;
		// Deserialize code
		for (auto i = 0; i < paraTypes.size(); ++i)
		{
			auto& t = paraTypes[i];
			argDeserialCodes.push_back(string("Json::JsonConverter::Deserialize<").append(t).append(">(jsonObj[").append(to_string(i)).append("])"));
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
			returnStatement = string("return Json::JsonObject();");
		}
		else
		{
			invokeStatement = string("auto r = std::apply(" + name + ", std::move(argsTuple));");
			returnStatement = string("return Json::JsonConverter::Serialize(r);");
		}

		wrapperFuncDef.push_back(move(argsTuple));
		wrapperFuncDef.push_back("// return type: " + returnType);
		wrapperFuncDef.push_back(move(invokeStatement));
		wrapperFuncDef.push_back(move(returnStatement));
		wrapperFuncDef.push_back("}");

		return wrapperFuncDef;
	}

	tuple<vector<vector<string>>, vector<FuncObj>, vector<string>> ProcessFuncs(vector<tuple<FuncType, vector<string>>> funcs)
	{
		vector<vector<string>> wrapperFuncsDef;
		vector<FuncObj> funcObjs;
		vector<string> headers
		{
			"Unity.hpp",
		};

		for (auto& f : funcs)
		{
			auto& type = get<0>(f);
			auto& paraNames = get<1>(f);

			wrapperFuncsDef.push_back(GenerateWrapperFunc(type, paraNames));
			funcObjs.push_back(FuncObj{move(type), move(paraNames), {}});
		}

		return tuple(move(wrapperFuncsDef), move(funcObjs), move(headers));
	};

	// 不太清楚 ParseFunc 在这个 operator 的返回值 decltype(auto) 推导出来是不是引用类型，
	// 是引用类型的话，应该是返回一个局部变量的，那就有问题了，但是编译器没有报 warning
	// 看了下编辑器鼠标放上去的提示，不是引用类型，之后补一下相关的知识
	decltype(auto) operator| (FuncsDefReader& reader, auto processor)
	{
		return processor(reader);
	}

	auto operator| (auto data, auto processor)
	{
		return processor(move(data));
	}

	string ReadAllCode(FuncsDefReader& reader)
	{
		reader.ResetReadPos();

		string code;
		code.reserve(200); // 估计的值，未仔细验证其有效性
		auto g = reader.GetLineCodeGenerator();
		while (g.MoveNext())
		{
			code.append(move(g.Current()));
		}

		return code;
	}

	pair<vector<FuncObj>, vector<char>> Compile(FuncsDefReader defReader)
	{
		auto [wrapperFuncsDef, funcObjs, headersToAdd] = defReader | CheckGrammar | ReadAllCode | ParseFunc | ProcessFuncs;
		
		AppendCode code{ move(headersToAdd), move(wrapperFuncsDef) };

#ifdef _MSVC_LANG
		auto bins = CompileOnWindows(defReader, &appendCode);
#else // __clang__ or __GNUC__
		auto bins = GetCompiledByteOnUnix(&defReader, &code);
#endif

		return { move(funcObjs), bins };
	}

	string GetWrapperFuncName(string_view rawName)
	{
		char const suffix[] = "_wrapper";
		string name(rawName);
		name.reserve(rawName.size()  + sizeof(suffix) - 1);
		name.append(suffix);
		return name;
	}
}
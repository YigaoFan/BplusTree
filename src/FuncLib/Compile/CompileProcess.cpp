
#include <array>
#include <string>
#include <string_view>
#include <cstdlib>
#include <fstream>
#include <random>
#include <algorithm>
#include <filesystem>
#include "CompileProcess.hpp"
#include "../../Basic/Exception.hpp"
#include "../../Btree/Generator.hpp"
#include "ParseFunc.hpp"

namespace FuncLib::Compile
{
	using Basic::InvalidOperationException;
	using Basic::NotImplementException;
	using Collections::RecursiveGenerator;
	using ::std::array;
	using ::std::get;
	using ::std::ifstream;
	using ::std::move;
	using ::std::ofstream;
	using ::std::string;
	using ::std::string_view;
	using ::std::to_string;
	using ::std::vector;
	using ::std::filesystem::remove;

	struct ExternCBody
	{
		vector<vector<string>> WrapperFuncDefs;
		RecursiveGenerator<string> GetLineCodeGenerator() const
		{
			for (auto& funcDef : WrapperFuncDefs)
			{
				for (auto& line : funcDef)
				{
					co_yield line;
				}
			}
		}
	};

	struct AppendCode
	{
		vector<string> IncludeNames;
		ExternCBody ExternCBody;

		RecursiveGenerator<string> GetLineCodeGenerator() const
		{
			for (auto& i :IncludeNames)
			{
				co_yield "#include \"" + i + "\"";
			}

			co_yield "extern \"C\"";
			co_yield "{";
			co_yield ExternCBody.GetLineCodeGenerator();
			co_yield "}";
		}
	};

	string RandomString()
	{
		string src = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		std::random_device rd;
		std::mt19937 generator(rd());

		std::shuffle(src.begin(), src.end(), generator);

		return src.substr(0, 5);
	}
	
	template <typename Generator>
	void AddCodeFrom(ofstream* file, Generator* generator)
	{
		while (generator->MoveNext())
		{
			*file << generator->Current();
		}
	}

	/// 返回编译过后的字节
	// read compiled file into memory
	vector<char> CompileOnUnix(FuncDefTokenReader* defReader, AppendCode* appendCode)
	{
		string filename = "temp_compile_" + RandomString();// 加入随机性
		ofstream f(filename + ".cpp");

		auto g1 = defReader->GetLineCodeGenerator();
		AddCodeFrom(&f, &g1);
		auto g2 = appendCode->GetLineCodeGenerator();
		AddCodeFrom(&f, &g2);

		auto compileCmd = string("g++ -m64 -g -shared-libgcc -fPIC -o ") + filename + ".so " + filename + ".cpp -std=c++0x -shared";
		system(compileCmd.c_str());

		ifstream binReader(filename + ".so", ifstream::binary);
		binReader.unsetf(ifstream::skipws); // Stop eating new lines in binary mode
		binReader.seekg(0, ifstream::end);
		auto size = binReader.tellg();
		binReader.clear();
		binReader.seekg(0, ifstream::beg);

		vector<char> bytes;
		bytes.reserve(size);
		bytes.insert(bytes.begin(),
					 std::istream_iterator<char>(binReader),
					 std::istream_iterator<char>());

		remove(filename + ".cpp");
		remove(filename + ".so");

		return bytes;
	}

	vector<char> CompileOnWindows(FuncDefTokenReader* defReader, AppendCode* appendCode)
	{
		throw NotImplementException("CompileOnWindows function not implemented");
	}

	/// Has ResetReadPos effect
	void CheckGrammar(FuncDefTokenReader* defReader)
	{
		string filename = "temp_pre_compile_" + RandomString();
		ofstream f(filename + ".cpp");

		defReader->ResetReadPos();
		auto g = defReader->GetLineCodeGenerator();
		AddCodeFrom(&f, &g);
		auto compileCmd = string("g++ -m64 -g -shared-libgcc -fPIC -o ") + filename + ".so " + filename + ".cpp -std=c++0x -shared";
		if (system(compileCmd.c_str()) != 0)
		{
			throw InvalidOperationException("function definitions have compile error");
		}

		remove(filename + ".cpp");
		remove(filename + ".so");
		defReader->ResetReadPos();
	}

	// add extern，extern 内只需要有 wrapper 函数
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
			"JsonObject " + name + "_wrapper(JsonObject jsonObj) {",
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
		for (auto i = 0; i < paraTypes.size(); ++i)
		{
			auto& t = paraTypes[i];
			auto& n = paraNames[i];
			argDeserialCodes.push_back(string("JsonConverter::Deserialize<") + t + ">(jsonObj[" + n + "])");
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
		wrapperFuncDef.push_back("}");

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
			auto& type = get<0>(f);
			auto& paraNames = get<1>(f);
			auto& body = get<2>(f);

			CheckProhibitedUseage(body);
			wrapperFuncsDef.push_back(GenerateWrapperFunc(type, paraNames));
			funcObjs.push_back(FuncObj{ move(type), move(paraNames), {} });
		}

		defReader->ResetReadPos();
		AppendCode appendCode;
		appendCode.IncludeNames.push_back("Json.hpp");
		appendCode.IncludeNames.push_back("tuple");
		appendCode.ExternCBody.WrapperFuncDefs = move(wrapperFuncsDef);
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
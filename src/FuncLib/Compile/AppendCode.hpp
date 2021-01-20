#pragma once
#include <vector>
#include <string>
#include "../../Btree/Generator.hpp"

namespace FuncLib::Compile
{
	using Collections::RecursiveGenerator;
	using ::std::string;
	using ::std::vector;

	// extern 内只有 wrapper 函数
	struct ExternCBody
	{
		vector<vector<string>> WrapperFuncDefs;
		RecursiveGenerator<string> GetLineCodeGenerator() const
		{
			for (auto &funcDef : WrapperFuncDefs)
			{
				for (auto& line : funcDef)
				{
					co_yield line + '\n';
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
			for (auto &i : IncludeNames)
			{
				co_yield "#include \"" + i + "\"\n";
			}

			co_yield "extern \"C\"\n";
			co_yield "{\n";
			co_yield ExternCBody.GetLineCodeGenerator();
			co_yield "}\n";
		}
	};
}

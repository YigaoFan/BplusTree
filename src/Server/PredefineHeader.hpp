#pragma once
#include <set>
#include <string>
#include <vector>
#include "../Btree/Generator.hpp"
#include "../FuncLib/Compile/FuncType.hpp"
#include "../Json/Json.hpp"
#include "../Network/Request.hpp"

namespace Server
{
	using Collections::Generator;
	using FuncLib::Compile::FuncType;
	using ::std::set;
	using ::std::string;
	using ::std::vector;

	using Json::JsonObject;
	using Json::JsonConverter::Serialize;
	using ::std::move;

	struct PredefineHeader
	{
		set<vector<string>> Packages;

		static PredefineHeader NewFrom(vector<FuncType> funcTypes)
		{
			PredefineHeader header;
			for (auto& t : funcTypes)
			{
				header.Packages.insert(move(t.Package));
			}

			return header;
		}

		static JsonObject GetFuncInfo(Generator<FuncType> funcsGenerator)
		{
			JsonObject::_Array a;
			while (funcsGenerator.MoveNext())
			{
				a.push_back(Serialize(funcsGenerator.Current()));
			}

			return JsonObject(move(a));
		}
		
		Generator<string> GetEnumDef()
		{
			co_yield "enum class PredefinePackage";
			co_yield "{";
			for (auto& p : Packages)
			{
				co_yield Join(p, '_') + ',';
			}

			co_yield "};";
		}

		Generator<string> GetToVecDef()
		{
			co_yield "vector<string> ToVec(PredefinePackage package)";
			co_yield "{";
			co_yield "switch (package)";
			co_yield "{";
			for (auto& p : Packages)
			{
				co_yield "case " + Join(p, '_') + ':';
				co_yield "return {" + Join(p, ',') + '}';
			}
			co_yield "{";
			co_yield "}";
		}

		Generator<string> GetHeaderDef()
		{
			vector<string> includes
			{
				"#pragma once",
				"#include <vector>",
				"#include <string>",
				"namespace RemoteInvokeLib",
				"{",
    			"using ::std::string;",
    			"using ::std::vector;",
			};

			for (auto& x : includes)
			{
				co_yield x;
			}

			vector<Generator<string>> defs;
			defs.push_back(GetEnumDef());
			defs.push_back(GetToVecDef());

			for (auto& x : defs)
			{
				while (x.MoveNext())
				{
					co_yield x.Current();
				}
			}

			co_yield "}";
		}

	private:
		static string Join(vector<string> const& strs, char c)
		{
			string joined;
			for (auto& s : strs)
			{
				joined.append(s + c);
			}

			if (not strs.empty())
			{
				joined.pop_back();
			}

			return joined;
		}
	};
}

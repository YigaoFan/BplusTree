#include <fstream>
#include <filesystem>
#include "../TestFrame/FlyTest.hpp"
#include "../Compile/CompileProcess.hpp"
#include "../Compile/ParseFunc.hpp"
#include "../Compile/SharedLibrary.hpp"
#include "../Json/Json.hpp"
#include "../Compile/Util.hpp"

using namespace FuncLib::Compile;
using namespace std;
using Json::JsonObject;

TESTCASE("CompileTest")
{
	SECTION("Check Braces Balance")
	{
		size_t i = 0;

		SECTION("Balance1")
		{
			ASSERT(CheckBracesBalance("{}", i).first);
		}

		SECTION("Balance2")
		{
			ASSERT(CheckBracesBalance("{ { {} } {}}", i).first);
		}

		SECTION("Balance3")
		{
			ASSERT(CheckBracesBalance("{ {  } } ", i).first);
		}

		SECTION("Balance4")
		{
			ASSERT(not CheckBracesBalance("{ { {} {}}", i).first);
		}

		// } 多的情况也会被判为 balance
		SECTION("Balance5")
		{
			ASSERT(not CheckBracesBalance("{ {  {}}", i).first);
		}

		SECTION("Balance5")
		{
			ASSERT(CheckBracesBalance("{ {  {}}}}", i).first);
		}
	}

	SECTION("Parse func")
	{
		SECTION("Simple")
		{
			auto funcs = ParseFunc("void Func(int a, int b) { }");
		}

		SECTION("Complex1")
		{
			auto code = "#include <string>\n"
						"using namespace std;\n"
						"void Func(int a, double b) { }";
			auto fs = ParseFunc(code);

			ASSERT(fs.size() == 1);
			auto f = fs[0];
			auto t = std::get<0>(f);
			ASSERT(t.FuncName == "Func");
			ASSERT(t.ArgTypes.size() == 2);
			ASSERT(t.ReturnType == "void");
		}

		SECTION("Complex2")
		{
			auto code = "#include <string>\n"
						"using namespace std;\n"
						"void Func1 (int a, double b) { }\n"
						"int Func(int a, double b) { }";
			auto fs = ParseFunc(code);

			ASSERT(fs.size() == 2);

			{
				auto f = fs[0];
				auto t = std::get<0>(f);
				ASSERT(t.FuncName == "Func1");
				ASSERT(t.ArgTypes.size() == 2);
				ASSERT(t.ReturnType == "void");
			}

			{
				auto f = fs[1];
				auto t = std::get<0>(f);
				ASSERT(t.FuncName == "Func");
				ASSERT(t.ArgTypes.size() == 2);
				ASSERT(t.ReturnType == "int");
			}
		}
	}

	SECTION("Compile")
	{
		using ::std::filesystem::exists;

		auto filename = "FuncDef.cpp";
		if (exists(filename))
		{
			ifstream ifs(filename, ifstream::in | ifstream::binary);
			auto reader = FuncsDefReader(make_unique<ifstream>(move(ifs)));
			auto [funcs, bytes] = Compile(move(reader));

			ASSERT(funcs.size() == 3);
			ASSERT(funcs[0].Type.FuncName == "Func");
			ASSERT(funcs[1].Type.FuncName == "Func2");
			ASSERT(funcs[2].Type.FuncName == "Func3");

			auto libName = "temp_shared_lib.so";
			ofstream ofs(libName, ofstream::out | ofstream::binary);
			FilesCleaner c(libName);
			ofs.write(bytes.data(), bytes.size());
			ofs.flush();

			SharedLibrary lib(libName);
			auto r = lib.Invoke<JsonObject(JsonObject)>("Func3_wrapper", JsonObject());
			ASSERT(r.IsNull());
		}
		else
		{
			printf("%s not exist, jump over CompileTest::Compile", filename);
		}
	}

	SECTION("FuncType")
	{
		{
			FuncType t("int", "One", {}, {});
			ASSERT(t.ToKey() == FuncType::FromKey(t.ToKey()).ToKey());
		}

		{
			FuncType t("void", "Foo", { "string" }, { "A", });
			ASSERT(t.ToKey() == FuncType::FromKey(t.ToKey()).ToKey());
		}

		{
			FuncType t("void", "Foo", {}, { "A", "B", });
			ASSERT(t.ToKey() == FuncType::FromKey(t.ToKey()).ToKey());
		}

		{
			FuncType t("void", "Foo", { "string", "int" }, {});
			ASSERT(t.ToKey() == FuncType::FromKey(t.ToKey()).ToKey());
		}

		{
			FuncType t("void", "Foo", { "string", "int" }, { "A", "B", });
			ASSERT(t.ToKey() == FuncType::FromKey(t.ToKey()).ToKey());
		}
	}
}

DEF_TEST_FUNC(TestCompile)
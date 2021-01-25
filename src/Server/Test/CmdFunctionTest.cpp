#include "../../TestFrame/FlyTest.hpp"
#define private public
#include "../CmdMetadata.hpp"

using namespace Server;

TESTCASE("Cmd Function Test")
{
	// 其实应该测试下异常情况报异常 TODO

	SECTION("Basic Util")
	{
		SECTION("Get Package")
		{
			{
				auto p = GetPackageFrom("A.B");
				ASSERT(p.size() == 2);
				ASSERT(p[0] == "A");
				ASSERT(p[1] == "B");
			}

			{
				auto p = GetPackageFrom("A");
				ASSERT(p.size() == 1);
				ASSERT(p[0] == "A");
			}

			{
				auto p = GetPackageFrom("");
				ASSERT(p.size() == 0);
			}
		}

		SECTION("Get FuncType")
		{
			{
				auto func = GetFuncTypeFrom("A.B::void Func(int, string)");

				auto &package = func.PackageHierarchy;
				ASSERT(package.size() == 2);
				ASSERT(package[0] == "A");
				ASSERT(package[1] == "B");

				ASSERT(func.ReturnType == "void");

				ASSERT(func.FuncName == "Func");

				auto &argTypes = func.ArgTypes;
				ASSERT(argTypes.size() == 2);
				ASSERT(argTypes[0] == "int");
				ASSERT(argTypes[1] == "string");
			}

			{
				auto func = GetFuncTypeFrom("B::void Func(int)");

				auto &package = func.PackageHierarchy;
				ASSERT(package.size() == 1);
				ASSERT(package[0] == "B");

				ASSERT(func.ReturnType == "void");

				ASSERT(func.FuncName == "Func");

				auto &argTypes = func.ArgTypes;
				ASSERT(argTypes.size() == 1);
				ASSERT(argTypes[0] == "int");
			}

			{
				auto func = GetFuncTypeFrom("B::void Func()");

				auto &package = func.PackageHierarchy;
				ASSERT(package.size() == 1);
				ASSERT(package[0] == "B");

				ASSERT(func.ReturnType == "void");

				ASSERT(func.FuncName == "Func");

				auto &argTypes = func.ArgTypes;
				ASSERT(argTypes.size() == 0);
			}

			{
				auto func = GetFuncTypeFrom("B::void Func ()");

				auto& package = func.PackageHierarchy;
				ASSERT(package.size() == 1);
				ASSERT(package[0] == "B");

				ASSERT(func.ReturnType == "void");

				ASSERT(func.FuncName == "Func");

				auto &argTypes = func.ArgTypes;
				ASSERT(argTypes.size() == 0);
			}

			{
				auto func = GetFuncTypeFrom("::void Func ()");

				auto &package = func.PackageHierarchy;
				ASSERT(package.size() == 0);

				ASSERT(func.ReturnType == "void");

				ASSERT(func.FuncName == "Func");

				auto &argTypes = func.ArgTypes;
				ASSERT(argTypes.size() == 0);
			}

			{
				auto func = GetFuncTypeFrom("AB.CD::void Func ()");

				auto &package = func.PackageHierarchy;
				ASSERT(package.size() == 2);
				ASSERT(package[0] == "AB");
				ASSERT(package[1] == "CD");

				ASSERT(func.ReturnType == "void");

				ASSERT(func.FuncName == "Func");

				auto &argTypes = func.ArgTypes;
				ASSERT(argTypes.size() == 0);
			}
		}

		SECTION("Preprocess")
		{
			{
				auto r = Preprocess("AddFunc", "AddFunc  a b");
				ASSERT(r == "a b");
			}

			{
				ASSERT_THROW(std::invalid_argument, Preprocess("AddFunc", "a b"));
			}
		}

		SECTION("Read def")
		{
			// auto def = AddFuncCmd::GetFuncsDefFrom("func.cpp");
			// printf("Debug %s\n", def.c_str());
		}
	}

	SECTION("AddFunc Cmd")
	{
		auto c = AddFuncCmd();
		auto [packagePart, filename, summary] = c.DivideInfo("A.B ./func.cpp basic util function");
		ASSERT(packagePart == "A.B");
		ASSERT(filename == "./func.cpp");
		ASSERT(summary == "basic util function");
	}

	SECTION("ModifyFuncPackage Cmd")
	{
		auto c = ModifyFuncPackageCmd();
		auto [funcInfo, newPackageInfo] = c.DivideInfo("A.B::void Func(int, string)");
		ASSERT(funcInfo == "void Func(int, string)");
		ASSERT(newPackageInfo == "A.B");
	}
}

DEF_TEST_FUNC(TestCmdFunction)
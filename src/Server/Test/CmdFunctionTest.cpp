#include "../../TestFrame/FlyTest.hpp"
#define private public
#include "../CmdFunction.hpp"

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

				auto &package = func.Package;
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

				auto &package = func.Package;
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

				auto &package = func.Package;
				ASSERT(package.size() == 1);
				ASSERT(package[0] == "B");

				ASSERT(func.ReturnType == "void");

				ASSERT(func.FuncName == "Func");

				auto &argTypes = func.ArgTypes;
				ASSERT(argTypes.size() == 0);
			}

			{
				auto func = GetFuncTypeFrom("B::void Func ()");

				auto& package = func.Package;
				ASSERT(package.size() == 1);
				ASSERT(package[0] == "B");

				ASSERT(func.ReturnType == "void");

				ASSERT(func.FuncName == "Func");

				auto &argTypes = func.ArgTypes;
				ASSERT(argTypes.size() == 0);
			}

			{
				auto func = GetFuncTypeFrom("::void Func ()");

				auto &package = func.Package;
				ASSERT(package.size() == 0);

				ASSERT(func.ReturnType == "void");

				ASSERT(func.FuncName == "Func");

				auto &argTypes = func.ArgTypes;
				ASSERT(argTypes.size() == 0);
			}

			{
				auto func = GetFuncTypeFrom("AB.CD::void Func ()");

				auto &package = func.Package;
				ASSERT(package.size() == 2);
				ASSERT(package[0] == "AB");
				ASSERT(package[1] == "CD");

				ASSERT(func.ReturnType == "void");

				ASSERT(func.FuncName == "Func");

				auto &argTypes = func.ArgTypes;
				ASSERT(argTypes.size() == 0);
			}
		}

		SECTION("Read def")
		{
			// auto def = AddFuncCmd::GetFuncsDefFrom("func.cpp");
			// printf("Debug %s\n", def.c_str());
		}
	}

	SECTION("AddFunc")
	{
		auto c = AddFuncCmd();
		auto [packagePart, filename, summary] = c.DivideInfo("A.B ./func.cpp basic util function");
		ASSERT(packagePart == "A.B");
		ASSERT(filename == "./func.cpp");
		ASSERT(summary == "basic util function");

		// auto r = c.ProcessArg("A.B ./func.cpp basic util function");
	}

	SECTION("RemoveFunc")
	{
		char const typeStr[] = "A.B::void Func(int, string)";
		auto type = GetFuncTypeFrom(typeStr);
		auto c = RemoveFuncCmd();
		auto r = c.ProcessArg(typeStr);
		using namespace Json;
		JsonObject::_Object _obj;
		_obj.insert({ "func", JsonConverter::Serialize(type) });
		JsonObject j(move(_obj));
		ASSERT(r.ToString() == j.ToString());
	}

	SECTION("SearchFunc")
	{
		auto c = SearchFuncCmd();
		auto r = c.ProcessArg("Func");
	}

	SECTION("ModifyFuncPackage")
	{
		auto c = ModifyFuncPackageCmd();
		auto [funcInfo, newPackageInfo] = c.DivideInfo("A.B::void Func(int, string) C.D");
		ASSERT(funcInfo == "A.B::void Func(int, string)");
		ASSERT(newPackageInfo == "C.D");
	}

	SECTION("ContainsFunc")
	{
		auto c = ContainsFuncCmd();
		auto r = c.ProcessArg("A.B::void Func(int, string)");
	}

	SECTION("AddClientAccount")
	{
		auto c = AddClientAccountCmd();
		auto r = c.ProcessArg("LiuQi 567");
	}

	SECTION("RemoveClientAccount")
	{
		auto c = RemoveClientAccountCmd();
		auto r = c.ProcessArg("LiuQi");
	}

	SECTION("AddAdminAccount")
	{
		auto c = AddAdminAccountCmd();
		auto r = c.ProcessArg("god 123");
	}

	SECTION("RemoveAdminAccount")
	{
		auto c = RemoveAdminAccountCmd();
		auto r = c.ProcessArg("god");
	}
}

DEF_TEST_FUNC(TestCmdFunction)
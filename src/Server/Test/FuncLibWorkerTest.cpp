#include "../TestFrame/FlyTest.hpp"
#include "../TestFrame/Util.hpp"
#include "../FuncLibWorker.hpp"
#include "Task.hpp"

namespace Svr = Server;
using namespace FuncLib;
using namespace Svr;
using namespace std;
using namespace Test;

namespace
{
	pair<vector<string>, FuncsDefReader> MakeFuncDefReader(string funcDef)
	{
		vector<string> package{ "Basic", };
		istringstream is(funcDef);
		auto reader = FuncsDefReader(make_unique<istringstream>(move(is)));
		return pair(move(package), move(reader));
	}
}

FuncType GetFuncType()
{
	auto f = FuncType("int", "Zero", {}, {"Basic"});
	return f;
}

void InitBaicFunc(FunctionLibrary& lib)
{
	auto def = "int Zero()\n"
			   "{\n"
			   "	return 0;\n"
			   "}\n";
	auto [package, defReader] = MakeFuncDefReader(def);
	lib.Add(package, move(defReader), "math zero");
}

InvokeFuncRequest::Content GetInvokeFuncRequestContent()
{
	return
	{
		GetFuncType(),
		Json::JsonObject(),
	};
}

AddFuncRequest::Content GetAddFuncRequestContent()
{
	vector<string> package{ "Basic", };
	auto def = "int One()\n"
			   "{\n"
			   "	return 1;\n"
			   "}\n";
	auto summary = "math one";

	return
	{
		package,
		def,
		summary,
	};
}

/// same to GetAddFuncRequestContent result's func
FuncType GetAddFuncType()
{
	vector<string> package{ "Basic", };
	return FuncType("int", "One", {}, package);
}

FuncType GetRemoveFuncType()
{
	vector<string> package{ "Basic", };
	return FuncType("int", "Zero", {}, package);
}

Task InvokeFunc(FuncLibWorker& libWorker, optional<JsonObject>& result)
{
	auto r = co_await libWorker.InvokeFunc(GetInvokeFuncRequestContent());
	result = move(r);
}

Task AddFunc(FuncLibWorker& libWorker)
{
	co_await libWorker.AddFunc(GetAddFuncRequestContent());
}

Task ContainsFunc(FuncLibWorker& libWorker, FuncType func, optional<bool>& result)
{
	auto r = co_await libWorker.ContainsFunc({ func, });
	result = move(r);
}

Task RemovePredefineFunc(FuncLibWorker& libWorker)
{
	co_await libWorker.RemoveFunc({ GetRemoveFuncType() });
}

FuncType GetModifiedFuncType()
{
	auto t = GetAddFuncType();
	t.PackageHierarchy = { "Math", };
	return t;
}

Task ModifyFuncPackage(FuncLibWorker& libWorker)
{
	co_await libWorker.ModifyFuncPackage({ GetAddFuncType(), { "Math", } });
}

TESTCASE("FuncLibWorker Test")
{
	Cleaner c1("./func.idx"), c2("./func_bin.lib");
	auto threadPool = ThreadPool(2);
	auto funcLib = FunctionLibrary::GetFrom(".");
	InitBaicFunc(funcLib);
	auto libWorker = FuncLibWorker(move(funcLib));
	libWorker.SetThreadPool(&threadPool);

	optional<JsonObject> invokeResult;
	auto t1 = InvokeFunc(libWorker, invokeResult);

	auto t2 = AddFunc(libWorker);
	auto t3 = RemovePredefineFunc(libWorker);
	t1.Wait();
	t2.Wait();
	optional<bool> containsResult;
	auto t4 = ContainsFunc(libWorker, GetAddFuncType(), containsResult);

	t3.Wait();
	optional<bool> removeVerifyResult;
	auto t5 = ContainsFunc(libWorker, GetRemoveFuncType(), removeVerifyResult);

	ASSERT(invokeResult.has_value());
	ASSERT(invokeResult->IsNumber());
	ASSERT(invokeResult->GetNumber() == 0);
	
	t4.Wait();
	t5.Wait();

	ASSERT(containsResult.has_value());
	ASSERT(containsResult.value());
	ASSERT(removeVerifyResult.has_value());
	ASSERT(not removeVerifyResult.value());

	auto t6 = ModifyFuncPackage(libWorker);
	t6.Wait();
	optional<bool> modifyResult;
	auto t7 = ContainsFunc(libWorker, GetModifiedFuncType(), modifyResult);
	optional<bool> verfiyModifyResult;
	auto t8 = ContainsFunc(libWorker, GetAddFuncType(), verfiyModifyResult);
	t7.Wait();
	ASSERT(modifyResult.has_value());
	ASSERT(modifyResult.value());
	t8.Wait();
	ASSERT(verfiyModifyResult.has_value());
	ASSERT(not verfiyModifyResult.value());
}

DEF_TEST_FUNC(TestFuncLibWorker)
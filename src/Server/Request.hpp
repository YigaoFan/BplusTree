#pragma once
#include <mutex>
#include <vector>
#include <string>
#include <functional>
#include <condition_variable>
#include "../FuncLib/Compile/FuncType.hpp"
#include "../FuncLib/Compile/FuncDefTokenReader.hpp"
#include "../Json/Json.hpp"

namespace Server
{
	using FuncLib::Compile::FuncDefTokenReader;
	using FuncLib::Compile::FuncType;
	using Json::JsonObject;
	using ::std::condition_variable;
	using ::std::function;
	using ::std::lock_guard;
	using ::std::mutex;
	using ::std::string;
	using ::std::vector;

	struct Request
	{
		Request() = default;

		Request(Request &&that) noexcept : Mutex(), CondVar(), Continuation()
		{
			lock_guard<mutex> guard(that.Mutex);
			Done = that.Done;
			Continuation = move(that.Continuation);
		}

		mutable mutex Mutex;
		mutable condition_variable CondVar;
		bool Done = false;
		function<void()> Continuation;
	};

	struct InvokeRequest : public Request
	{
		FuncType Func;
		JsonObject Arg;
		JsonObject Result;
	};

	struct AddFuncRequest : public Request
	{
		vector<string> Package;
		FuncDefTokenReader DefReader;
		string Summary;
	};

	struct RemoveFuncRequest : public Request
	{
		FuncType Func;
	};

	struct SearchFuncRequest : public Request
	{
		string Keyword;
		// 暂时 Result 是这个类型吧，之后看能不能改成 Generator 形式 TODO
		vector<pair<string, string>> Result;
	};
}

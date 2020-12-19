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

	struct InvokeFuncRequest : public Request
	{
		struct Content
		{
			FuncType Func;
			JsonObject Arg;
		};

		Content Paras;
		JsonObject Result;
	};

	struct AddFuncRequest : public Request
	{
		struct Content
		{
			vector<string> Package;
			string FuncsDef;
			string Summary;
		};

		Content Paras;
	};

	struct RemoveFuncRequest : public Request
	{
		struct Content
		{
			FuncType Func;
		};

		Content Paras;
	};

	struct SearchFuncRequest : public Request
	{
		struct Content
		{
			string Keyword;
		};

		Content Paras;
		// 暂时 Result 是这个类型吧，之后看能不能改成 Generator 形式 TODO
		vector<pair<string, string>> Result;
	};
}

namespace Json::JsonConverter
{
	using FuncLib::Compile::FuncType;
	using Server::AddFuncRequest;
	using Server::InvokeFuncRequest;
	using Server::RemoveFuncRequest;
	using Server::SearchFuncRequest;
	using ::std::move;

#define nameof(VAR) #VAR
	// Serialize 和 Deserialize 里的 item 名字需要一样

	template <>
	JsonObject Serialize(FuncType const &type)
	{
		auto [returnType, funcName, argTypes, package] = type;
		JsonObject::_Object obj;
		obj.insert({ nameof(returnType), Serialize(returnType) });
		obj.insert({ nameof(funcName),   Serialize(funcName) });
		obj.insert({ nameof(argTypes),   Serialize(argTypes) });
		obj.insert({ nameof(package),    Serialize(package) });

		return JsonObject(move(obj));
	}

	template <>
	FuncType Deserialize(JsonObject const& jsonObj)
	{
		auto returnType = Deserialize<string>(jsonObj[nameof(returnType)]);
		auto funcName = Deserialize<string>(jsonObj[nameof(funcName)]);
		auto argTypes = Deserialize<vector<string>>(jsonObj[nameof(argTypes)]);
		auto package = Deserialize<vector<string>>(jsonObj[nameof(package)]);

		return FuncType(returnType, funcName, argTypes, package);
	}

	///---------- InvokeFuncRequest ----------
	template <>
	JsonObject Serialize(InvokeFuncRequest::Content const& content)
	{
		auto [func, arg] = content;
		JsonObject::_Object obj;
		obj.insert({ nameof(func), Serialize(func) });
		obj.insert({ nameof(arg),  arg });

		return JsonObject(move(obj));
	}

	template <>
	InvokeFuncRequest::Content Deserialize(JsonObject const& jsonObj)
	{
		auto func = Deserialize<FuncType>(jsonObj[nameof(func)]);
		auto arg = jsonObj[nameof(arg)];
		
		return { move(func), move(arg) };
	}

	///---------- AddFuncRequest ----------
	template <>
	JsonObject Serialize(AddFuncRequest::Content const& content)
	{
		auto [package, funcsDef, summary] = content;
		JsonObject::_Object obj;
		obj.insert({ nameof(package), Serialize(package) });
		obj.insert({ nameof(funcsDef), Serialize(funcsDef) });
		obj.insert({ nameof(summary), Serialize(summary) });

		return JsonObject(move(obj));
	}

	template <>
	AddFuncRequest::Content Deserialize(JsonObject const& jsonObj)
	{
		auto package = Deserialize<vector<string>>(jsonObj[nameof(package)]);
		auto funcsDef = Deserialize<string>(jsonObj[nameof(funcsDef)]);
		auto summary = Deserialize<string>(jsonObj[nameof(summary)]);
		
		return { move(package), move(funcsDef), move(summary) };
	}

	///---------- RemoveFuncRequest ----------
	template <>
	JsonObject Serialize(RemoveFuncRequest::Content const& content)
	{
		auto [func] = content;
		JsonObject::_Object obj;
		obj.insert({ nameof(func), Serialize(func) });

		return JsonObject(move(obj));
	}

	template <>
	RemoveFuncRequest::Content Deserialize(JsonObject const& jsonObj)
	{
		auto func = Deserialize<FuncType>(jsonObj[nameof(func)]);
		
		return { move(func) };
	}

	///---------- SearchFuncRequest ----------
	template <>
	JsonObject Serialize(SearchFuncRequest::Content const& content)
	{
		auto [keyword] = content;
		JsonObject::_Object obj;
		obj.insert({ nameof(keyword), Serialize(keyword) });

		return JsonObject(move(obj));
	}

	template <>
	SearchFuncRequest::Content Deserialize(JsonObject const& jsonObj)
	{
		auto keyword = Deserialize<string>(jsonObj[nameof(keyword)]);
		
		return { move(keyword) };
	}

	///---------- ModifyFuncRequest ----------
	// template <>
	// JsonObject Serialize(InvokeFuncRequest::Content const& content)
	// {
	// 	auto [func, arg] = content;
	// 	JsonObject::_Object obj;
	// 	obj.insert({ nameof(func), Serialize(func) });
	// 	obj.insert({ nameof(arg),  arg });

	// 	return JsonObject(move(obj));
	// }

	// template <>
	// InvokeFuncRequest::Content Deserialize(JsonObject const& jsonObj)
	// {
	// 	auto func = Deserialize<FuncType>(jsonObj[nameof(func)]);
	// 	auto arg = jsonObj[nameof(arg)];
		
	// 	return { move(func), move(arg) };
	// }
#undef nameof
}
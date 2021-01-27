#pragma once
#include <mutex>
#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <condition_variable>
#include "../Json/Json.hpp"
#include "../FuncLib/Compile/FuncType.hpp"
#include "../Json/JsonConverter/JsonConverter.hpp"

namespace Server
{
	using FuncLib::Compile::FuncType;
	using Json::JsonObject;
	using ::std::condition_variable;
	using ::std::function;
	using ::std::mutex;
	using ::std::pair;
	using ::std::string;
	using ::std::vector;

	struct Request
	{
		mutable mutex Mutex;
		mutable condition_variable CondVar;
		bool Done = false;
		// Continuation
		function<void()> Success;
		function<void()> Fail;

		Request() = default;
		Request(Request &&that) noexcept;
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

	struct ModifyFuncPackageRequest : public Request
	{
		struct Content
		{
			FuncType Func;
			vector<string> NewPackage;
		};

		Content Paras;
	};

	struct ContainsFuncRequest : public Request
	{
		struct Content
		{
			FuncType Func;
		};

		Content Paras;
		bool Result;
	};
}

namespace Json::JsonConverter
{
	using FuncLib::Compile::FuncType;
	using Server::AddFuncRequest;
	using Server::InvokeFuncRequest;
	using Server::ModifyFuncPackageRequest;
	using Server::RemoveFuncRequest;
	using Server::SearchFuncRequest;
	using Server::ContainsFuncRequest;

	///---------- FuncType ----------
	template <>
	JsonObject Serialize(FuncType const &type);

	template <>
	FuncType Deserialize(JsonObject const& jsonObj);

	///---------- InvokeFuncRequest ----------
	template <>
	JsonObject Serialize(InvokeFuncRequest::Content const& content);

	template <>
	InvokeFuncRequest::Content Deserialize(JsonObject const& jsonObj);

	///---------- AddFuncRequest ----------
	template <>
	JsonObject Serialize(AddFuncRequest::Content const& content);

	template <>
	AddFuncRequest::Content Deserialize(JsonObject const& jsonObj);

	///---------- RemoveFuncRequest ----------
	template <>
	JsonObject Serialize(RemoveFuncRequest::Content const& content);

	template <>
	RemoveFuncRequest::Content Deserialize(JsonObject const& jsonObj);

	///---------- SearchFuncRequest ----------
	template <>
	JsonObject Serialize(SearchFuncRequest::Content const& content);

	template <>
	SearchFuncRequest::Content Deserialize(JsonObject const& jsonObj);

	///---------- ModifyFuncPackageRequest ----------
	template <>
	JsonObject Serialize(ModifyFuncPackageRequest::Content const& content);

	template <>
	ModifyFuncPackageRequest::Content Deserialize(JsonObject const& jsonObj);

	///---------- ContainsFuncPackageRequest ----------
	template <>
	JsonObject Serialize(ContainsFuncRequest::Content const& content);

	template <>
	ContainsFuncRequest::Content Deserialize(JsonObject const& jsonObj);
}
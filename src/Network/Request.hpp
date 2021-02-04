#pragma once
#include <mutex>
#include <vector>
#include <string>
#include <utility>
#include <exception>
#include <functional>
#include <condition_variable>
#include "../Json/Json.hpp"
#include "../FuncLib/Compile/FuncType.hpp"
#include "../Json/JsonConverter/JsonConverter.hpp"

namespace Network
{
	using FuncLib::Compile::FuncType;
	using Json::JsonObject;
	using ::std::condition_variable;
	using ::std::exception_ptr;
	using ::std::function;
	using ::std::mutex;
	using ::std::pair;
	using ::std::string;
	using ::std::vector;

	enum AdminServiceOption
	{
		AddFunc,
		RemoveFunc,
		SearchFunc,
		ModifyFuncPackage,
		ContainsFunc,
		AddClientAccount,
		RemoveClientAccount,
		AddAdminAccount,
		RemoveAdminAccount,
		GetFuncsInfo,
		Shutdown,
	};

	/// Contain request handle thing
	struct Request
	{
		mutable mutex Mutex;
		mutable condition_variable CondVar;
		bool Done = false;
		// Continuation
		function<void()> Continuation;
		function<void(exception_ptr)> RegisterException;

		Request() = default;
		Request(Request&& that) noexcept;
	};

	///---------- FuncLibWorker request ----------

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

	struct GetFuncsInfoRequest : public Request
	{
		vector<FuncType> Result;
	};

	///---------- AccountManager request ----------

	struct LoginRequest
	{
		struct Content
		{
			string Username;
			string Password;
		};

		Content Paras;
	};

	struct AddClientAccountRequest : public Request
	{
		struct Content
		{
			string Username;
			string Password;
		};

		Content Paras;
	};

	struct RemoveClientAccountRequest : public Request
	{
		struct Content
		{
			string Username;
		};

		Content Paras;
	};

	struct AddAdminAccountRequest : public Request
	{
		struct Content
		{
			string Username;
			string Password;
		};

		Content Paras;
	};

	struct RemoveAdminAccountRequest : public Request
	{
		struct Content
		{
			string Username;
		};

		Content Paras;
	};
}

namespace Json::JsonConverter
{
	using FuncLib::Compile::FuncType;
	using Network::AddAdminAccountRequest;
	using Network::AddClientAccountRequest;
	using Network::AddFuncRequest;
	using Network::AdminServiceOption;
	using Network::ContainsFuncRequest;
	using Network::InvokeFuncRequest;
	using Network::LoginRequest;
	using Network::ModifyFuncPackageRequest;
	using Network::RemoveAdminAccountRequest;
	using Network::RemoveClientAccountRequest;
	using Network::RemoveFuncRequest;
	using Network::SearchFuncRequest;

	///---------- FuncType ----------
	template <>
	JsonObject Serialize(FuncType const& type);

	template <>
	FuncType Deserialize(JsonObject const& jsonObj);

	///---------- AdminServiceOption ----------
	template <>
	JsonObject Serialize(AdminServiceOption const &option);

	template <>
	AdminServiceOption Deserialize(JsonObject const &jsonObj);

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

	///---------- LoginRequest ----------
	template <>
	JsonObject Serialize(LoginRequest::Content const& content);

	template <>
	LoginRequest::Content Deserialize(JsonObject const& jsonObj);

	///---------- AddClientAccountRequest ----------
	template <>
	JsonObject Serialize(AddClientAccountRequest::Content const& content);

	template <>
	AddClientAccountRequest::Content Deserialize(JsonObject const& jsonObj);

	///---------- AddAdminAccountRequest ----------
	template <>
	JsonObject Serialize(AddAdminAccountRequest::Content const& content);

	template <>
	AddAdminAccountRequest::Content Deserialize(JsonObject const& jsonObj);

	///---------- RemoveClientAccountRequest ----------
	template <>
	JsonObject Serialize(RemoveClientAccountRequest::Content const& content);

	template <>
	RemoveClientAccountRequest::Content Deserialize(JsonObject const& jsonObj);

	///---------- RemoveAdminAccountRequest ----------
	template <>
	JsonObject Serialize(RemoveAdminAccountRequest::Content const& content);

	template <>
	RemoveAdminAccountRequest::Content Deserialize(JsonObject const& jsonObj);
}
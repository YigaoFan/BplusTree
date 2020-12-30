#include "Request.hpp"

namespace Server
{
	using ::std::lock_guard;
	
	Request::Request(Request &&that) noexcept : Mutex(), CondVar(), Success()
	{
		lock_guard<mutex> guard(that.Mutex);
		Done = that.Done;
		Success = move(that.Success);
		Fail = move(that.Fail);
	}
}

namespace Json::JsonConverter
{
	using ::std::move;

#define nameof(VAR) #VAR
	// Serialize 和 Deserialize 里的 item 名字需要一样


	///---------- FuncType ----------
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

	///---------- ModifyFuncPackageRequest ----------
	template <>
	JsonObject Serialize(ModifyFuncPackageRequest::Content const& content)
	{
		auto [func, newPackage] = content;
		JsonObject::_Object obj;
		obj.insert({ nameof(func), Serialize(func) });
		obj.insert({ nameof(newPackage), Serialize(newPackage) });

		return JsonObject(move(obj));
	}

	template <>
	ModifyFuncPackageRequest::Content Deserialize(JsonObject const& jsonObj)
	{
		auto func = Deserialize<FuncType>(jsonObj[nameof(func)]);
		auto newPackage = Deserialize<vector<string>>(jsonObj[nameof(newPackage)]);

		return { move(func), move(newPackage) };
	}
#undef nameof
}
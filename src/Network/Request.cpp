#include "Request.hpp"

namespace Network
{
	using ::std::lock_guard;
	
	Request::Request(Request&& that) noexcept : Mutex(), CondVar(), Continuation()
	{
		lock_guard<mutex> guard(that.Mutex);
		Done = that.Done;
		Continuation = move(that.Continuation);
		RegisterException = move(that.RegisterException);
	}
}

namespace Json::JsonConverter
{
	using ::std::move;

#define nameof(VAR) #VAR
	// Serialize 和 Deserialize 里的 item 名字需要一样

	///---------- FuncType ----------
	template <>
	JsonObject Serialize(FuncType const& type)
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

	///---------- AdminServiceOption ----------
	template <>
	JsonObject Serialize(AdminServiceOption const& option)
	{
		return Serialize(static_cast<int>(option));
	}

	template <>
	AdminServiceOption Deserialize(JsonObject const& jsonObj)
	{
		return static_cast<AdminServiceOption>(Deserialize<int>(jsonObj));
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

	///---------- ContainsFuncPackageRequest ----------
	template <>
	JsonObject Serialize(ContainsFuncRequest::Content const& content)
	{
		auto [func] = content;
		JsonObject::_Object obj;
		obj.insert({ nameof(func), Serialize(func) });

		return JsonObject(move(obj));
	}

	template <>
	ContainsFuncRequest::Content Deserialize(JsonObject const& jsonObj)
	{
		auto func = Deserialize<FuncType>(jsonObj[nameof(func)]);

		return { move(func) };
	}

	///---------- Account Info Serialize Deserialize Helper ----------

	JsonObject AccountInfoSerialize(auto const& content)
	{
		auto [username, password] = content;
		JsonObject::_Object obj;
		obj.insert({ nameof(username), Serialize(username) });
		obj.insert({ nameof(password), Serialize(password) });

		return JsonObject(move(obj));
	}

	template <typename T>
	T AccountInfoDeserialize(JsonObject const& jsonObj)
	{
		auto username = Deserialize<string>(jsonObj[nameof(username)]);
		auto password = Deserialize<string>(jsonObj[nameof(password)]);

		return { move(username), move(password) };
	}

	///---------- LoginRequest ----------
	template <>
	JsonObject Serialize(LoginRequest::Content const& content)
	{
		auto [username, password] = content;
		JsonObject::_Object obj;
		obj.insert({ nameof(username), Serialize(username) });
		obj.insert({ nameof(password), Serialize(password) });

		return JsonObject(move(obj));
	}

	template <>
	LoginRequest::Content Deserialize(JsonObject const& jsonObj)
	{
		auto username = Deserialize<string>(jsonObj[nameof(username)]);
		auto password = Deserialize<string>(jsonObj[nameof(password)]);

		return { username, password };
	}

	///---------- AddClientAccountRequest ----------
	template <>
	JsonObject Serialize(AddClientAccountRequest::Content const& content)
	{
		return AccountInfoSerialize(content);
	}

	template <>
	AddClientAccountRequest::Content Deserialize(JsonObject const& jsonObj)
	{
		return AccountInfoDeserialize<AddClientAccountRequest::Content>(jsonObj);
	}

	///---------- AddAdminAccountRequest ----------
	template <>
	JsonObject Serialize(AddAdminAccountRequest::Content const& content)
	{
		return AccountInfoSerialize(content);
	}

	template <>
	AddAdminAccountRequest::Content Deserialize(JsonObject const& jsonObj)
	{
		return AccountInfoDeserialize<AddAdminAccountRequest::Content>(jsonObj);
	}

	///---------- Account Username Serialize Deserialize Helper ----------
	JsonObject AccountUsernameSerialize(auto const& content)
	{
		auto [username] = content;
		JsonObject::_Object obj;
		obj.insert({ nameof(username), Serialize(username) });

		return JsonObject(move(obj));
	}

	template <typename T>
	T AccountUsernameDeserialize(JsonObject const& jsonObj)
	{
		auto username = Deserialize<string>(jsonObj[nameof(username)]);

		return { move(username), };
	}

	///---------- RemoveClientAccountRequest ----------
	template <>
	JsonObject Serialize(RemoveClientAccountRequest::Content const& content)
	{
		return AccountUsernameSerialize(content);
	}

	template <>
	RemoveClientAccountRequest::Content Deserialize(JsonObject const& jsonObj)
	{
		return AccountUsernameDeserialize<RemoveClientAccountRequest::Content>(jsonObj);
	}

	///---------- RemoveAdminAccountRequest ----------
	template <>
	JsonObject Serialize(RemoveAdminAccountRequest::Content const& content)
	{
		return AccountUsernameSerialize(content);
	}

	template <>
	RemoveAdminAccountRequest::Content Deserialize(JsonObject const& jsonObj)
	{
		return AccountUsernameDeserialize<RemoveAdminAccountRequest::Content>(jsonObj);
	}
#undef nameof
}
#pragma once
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <filesystem>
#include "../Json/Json.hpp"
#include "Compile/FuncsDefReader.hpp"
#include "FuncBinaryLib.hpp"
#include "FuncBinaryLibIndex.hpp"
#include "../Btree/Generator.hpp"
#include "Compile/CompileProcess.hpp"

namespace FuncLib
{
	using Collections::Generator;
	using FuncLib::Compile::FuncsDefReader;
	using Json::JsonObject;
	using ::std::pair;
	using ::std::string;
	using ::std::unordered_map;
	using ::std::vector;
	using ::std::filesystem::path;

	struct FuncTypeHash
	{
		size_t operator()(FuncLib::FuncType const &t) const
		{
			auto h = ::std::hash<string>();
			return h(t.ToKey());
		}
	};

	struct FuncTypeEqualTo
	{
		bool operator() (FuncLib::FuncType const &left, FuncLib::FuncType const &right) const
		{
			auto h = FuncTypeHash();
			return h(left) == h(right);
		}
	};

	class FunctionLibrary
	{
	private:
		using InvokeFuncType = JsonObject(JsonObject);
		//  hash map 作为缓存，快速查询
		unordered_map<FuncType, pos_label, FuncTypeHash, FuncTypeEqualTo> _funcInfoCache;
		FuncBinaryLibIndex _index;
		FuncBinaryLib _binLib;

		FunctionLibrary(decltype(_index) index, decltype(_binLib) binLib);

	public:
		static FunctionLibrary GetFrom(path dirPath);
		/// 当加入多个项，中间某一个项抛异常，保证之前添加进去的项正常
		void Add(vector<string> package, FuncsDefReader defReader, string summary);
		bool Contains(FuncType const& func) const;
		void ModifyPackageOf(FuncType const& func, vector<string> package);
		void Remove(FuncType const& func);
		/// 有异常会抛出
		JsonObject Invoke(FuncType const& func, JsonObject args);
		/// pair: FuncType.ToKey(), summary
		Generator<pair<string, string>> Search(string const& keyword) const;
		Generator<FuncType> FuncTypes() const;
		auto GetInvoker(FuncType func, JsonObject args)
		{
			auto l = GetStoreLabel(func);
			auto libPtr = _binLib.Load(l);
			return [funcName=move(func.FuncName), libPtr=move(libPtr), args=move(args)]() -> JsonObject
			{
				auto wrapperFuncName = Compile::GetWrapperFuncName(funcName);
				return libPtr->Invoke<InvokeFuncType>(wrapperFuncName.c_str(), move(args));
			};
		}

	private:
		pos_label GetStoreLabel(FuncType const& func);
	};
}

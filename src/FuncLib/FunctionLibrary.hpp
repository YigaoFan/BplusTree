#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <filesystem>
#include <unordered_map>
#include "../Json/Json.hpp"
#include "CompileProcess.hpp"
#include "FuncBodyLib.hpp"
#include "FuncBodyLibIndex.hpp"

namespace FuncLib
{
	using Json::JsonObject;
	using ::std::pair;
	using ::std::string;
	using ::std::tuple;
	using ::std::unordered_map;
	using ::std::vector;
	using ::std::filesystem::path;

	class FunctionLibrary
	{
	private:
		// 这里有个 hash map 作为缓存，快速查询
		// 下面这个作为 key 可能需要一个 hash<FuncType> 的特化
		unordered_map<FuncType, pair<pos_int, size_t>> _posCache;
		FuncBodyLibIndex _index;
		FuncBodyLib _bodyLib;

		static FunctionLibrary Get(string libName)
		{
			// if exist, load
			// or create
		}

		FunctionLibrary(string libName)
		{
		}

	public:
		void Add(string const& packageName, path const& dirPath)
		{
			// 暂定一个函数对应一个 dll 文件，多对一加载 dll 的时候可能要想一下
			// 扫描某个目录，批量添加函数，以文件中的函数名为函数名（结合提供的 packageName）
		}

		void Add(string packageName, vector<string> funcDef, string summary)
		{
			auto fun = Compile(funcDef);
			fun.Type().PackageName(packageName);
			auto p = _bodyLib.Add(fun.BodyBytes());
			_index.Add(p, fun.Type(), move(summary));
		}

		void ReplaceName(string funcName, string newFuncName)
		{

		}

		void ReplaceSummary(string funcName, string newSummary)
		{

		}

		/// FuncName: A.B.Func
		void ReplaceDef(string funcName, vector<string> newFuncDef)
		{
			auto fun = Compile(newFuncDef);
			
		}

		/// A.B.Func
		void Remove(string const& funcName)
		{
		}

		JsonObject Invoke(string const& funcName, JsonObject args)
		{
			FuncType t;// 从 funcName 构造出 FuncType
			auto [pos, size] = GetStoreInfo(t);
			auto bytes = _bodyLib.Read(pos, size);
			// load byte and call func
			// return result
		}

		// pair: funcName, func type, func summary
		// keyword maybe part package name, 需要去匹配，所以返回值可能不能做到返回函数的相关信息
		vector<tuple<string, string, string>> Search(string const& keyword)
		{
			
		}
	private:
		pair<pos_int, size_t> GetStoreInfo(FuncType const &type)
		{
			if (_posCache.contains(type))
			{
				return _posCache[type];
			}

			return _index.GetStoreInfo(type);
		}
	}
}

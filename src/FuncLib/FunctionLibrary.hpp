#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <filesystem>
#include <unordered_map>
#include "../Json/Json.hpp"
#include "Compile/CompileProcess.hpp"
#include "Compile/FuncDefTokenReader.hpp"
#include "FuncBinaryLib.hpp"
#include "FuncBinaryLibIndex.hpp"

namespace FuncLib
{
	using FuncLib::Compile::Compile;
	using FuncLib::Compile::FuncDefTokenReader;
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
		// 这里的查询结果可能要包含 summary？
		unordered_map<FuncType, pair<pos_int, size_t>> _funcInfoCache;
		FuncBinaryLibIndex _index;
		FuncBinaryLib _binLib;

		static FunctionLibrary Get(string libName)
		{
			// if exist, load
			// or create
		}

		FunctionLibrary(string libName)
		{
		}

	public:
		// 这个功能让上层（脚本命令行）来做
		// void Add(string const& packageName, path const& dirPath)
		// {
		// 	// 暂定一个函数对应一个 dll 文件，多对一加载 dll 的时候可能要想一下
		// 	// 扫描某个目录，批量添加函数，以文件中的函数名为函数名（结合提供的 packageName）
		// }

		/// 加入多个项，如果中间某一个项爆异常，那就会处于一个中间状态了，让用户来处理，
		/// 所以需要有个 Contains 作用的函数在这个类中
		void Add(vector<string> packageHierarchy, FuncDefTokenReader defReader, string summary)
		{
			auto [funcs, bin] = Compile(&defReader);
			auto p = _binLib.Add(bin);

			for (auto& f : funcs)
			{
				f.Type.PackageHierarchy(packageHierarchy);
				if (not _index.Contains(f.Type))
				{
					// 那这样要考虑多个引用一个的问题，删的时候要注意
					_binLib.AddRefCount(p);
					_index.Add(p, f);
				}
				else
				{
					// throw exception
				}	
			}
		}

		// 是用 type 这种，把组装对象的逻辑放在外面，还是 vector<string> packageHierarchy, string funcName，把组装的逻辑放在这里
		void ModifyFuncName(FuncType type, string newFuncName)
		{

		}

		void ModifyPackageNameOfFunc(FuncType type, vector<string> packageHierarchy)
		{
		}

		/// A.B.Func
		void Remove(FuncType type)
		{
		}

		/// 由外面处理异常
		JsonObject Invoke(FuncType type, JsonObject args)
		{
			auto [pos, size] = GetStoreInfo(type);
			auto bytes = _binLib.Read(pos, size);
			// load byte and call func
			
			// return result
		}

		// pair: funcName, func type, func summary
		// keyword maybe part package name, 需要去匹配，所以返回值可能不能做到返回函数的相关信息
		vector<tuple<string, string, string>> Search(string const& keyword)
		{
			
		}
	private:
		pair<pos_int, size_t> GetStoreInfo(FuncType const& type)
		{
			if (_funcInfoCache.contains(type))
			{
				return _funcInfoCache[type];
			}

			auto info = _index.GetStoreInfo(type);
			_funcInfoCache.insert({ type, info });
			return info;
		}
	}
}

#pragma once
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <filesystem>
#include "../Json/Json.hpp"
#include "Compile/FuncDefTokenReader.hpp"
#include "FuncBinaryLib.hpp"
#include "FuncBinaryLibIndex.hpp"
#include "../Btree/Generator.hpp"

namespace FuncLib
{
	using Collections::Generator;
	using FuncLib::Compile::FuncDefTokenReader;
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
		//  hash map 作为缓存，快速查询
		unordered_map<FuncType, pos_label, FuncTypeHash, FuncTypeEqualTo> _funcInfoCache;
		FuncBinaryLibIndex _index;
		FuncBinaryLib _binLib;

		FunctionLibrary(decltype(_index) index, decltype(_binLib) binLib);

	public:
		static FunctionLibrary GetFrom(path dirPath);
		// 批量 Add 功能让上层（脚本命令行）来做
		// void Add(string const& packageName, path const& dirPath)
		// {
		// 	// 暂定一个函数对应一个 dll 文件，多对一加载 dll 的时候可能要想一下
		// 	// 扫描某个目录，批量添加函数，以文件中的函数名为函数名（结合提供的 packageName）
		// }

		/// 加入多个项，如果中间某一个项爆异常，那就会处于一个中间状态了，让用户来处理，
		void Add(vector<string> packageHierarchy, FuncDefTokenReader defReader, string summary);
		bool Contains(FuncType const& type) const;
		// 是用 type 这种，把组装对象的逻辑放在外面，还是 vector<string> packageHierarchy, string funcName，把组装的逻辑放在这里
		// void ModifyFuncName(FuncType const& type, string newFuncName);
		void ModifyPackageNameOf(FuncType const& type, vector<string> packageHierarchy);
		void Remove(FuncType const& type);
		/// 由外面处理异常
		JsonObject Invoke(FuncType const& type, JsonObject args);
		// keyword maybe part package name, 需要去匹配，所以返回值可能不能做到返回函数的相关信息
		Generator<pair<string, string>> Search(string const& keyword);

	private:
		pos_label GetStoreLabel(FuncType const& type);
	};
}

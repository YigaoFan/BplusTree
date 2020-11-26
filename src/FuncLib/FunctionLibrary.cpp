#include "FunctionLibrary.hpp"
#include "../Basic/Exception.hpp"
#include "Compile/CompileProcess.hpp"

namespace FuncLib
{
	using Basic::InvalidOperationException;
	using ::std::move;

	FunctionLibrary::FunctionLibrary(decltype(_index) index, decltype(_binLib) binLib)
		: _index(move(index)), _binLib(move(binLib))
	{
	}

	FunctionLibrary FunctionLibrary::GetFrom(path const& dirPath)
	{
		// TODO
		auto indexFilePath = dirPath;
		auto binFilePath = dirPath;
		auto i = FuncBinaryLibIndex::GetFrom(indexFilePath);
		auto b = FuncBinaryLib::GetFrom(binFilePath);
		return FunctionLibrary(move(i), move(b));
	}

	/// 加入多个项，如果中间某一个项爆异常，那就会处于一个中间状态了，让用户来处理，
	/// 所以需要有个 Contains 作用的函数在这个类中
	void FunctionLibrary::Add(vector<string> packageHierarchy, FuncDefTokenReader defReader, string summary)
	{
		auto [funcs, bin] = Compile::Compile(&defReader);
		auto p = _binLib.Add(move(bin));

		for (auto& f : funcs)
		{
			f.Type.PackageHierarchy(packageHierarchy);
			if (_index.Contains(f.Type))
			{
				// TODO 给异常加上出错的函数名
				throw InvalidOperationException("Function already exist");
			}
			else
			{
				// 那这样要考虑多个引用一个的问题，删的时候要注意
				_binLib.AddRefCount(p);
				_index.Add(p.Label(), f.Type);// 哪里存 summary ？
			}
		}
	}

	// 是用 type 这种，把组装对象的逻辑放在外面，还是 vector<string> packageHierarchy, string funcName，把组装的逻辑放在这里
	void FunctionLibrary::ModifyFuncName(FuncType const& type, string newFuncName)
	{
		if (_index.Contains(type))
		{
			return _index.ModifyFuncName(type, move(newFuncName));
		}

		throw InvalidOperationException("Function not exist");
	}

	void FunctionLibrary::ModifyPackageNameOf(FuncType const &type, vector<string> packageHierarchy)
	{
		if (_index.Contains(type))
		{
			return _index.ModifyPackageOf(type, move(packageHierarchy));
		}

		throw InvalidOperationException("Function not exist");
	}

	void FunctionLibrary::Remove(FuncType const &type)
	{
		if (_index.Contains(type))
		{
			auto l = GetStoreLabel(type);
			_binLib.DecreaseRefCount(l);
			return _index.Remove(type);
		}

		throw InvalidOperationException("Function not exist");
	}

	/// 由外面处理异常
	JsonObject FunctionLibrary::Invoke(FuncType const& type, JsonObject args)
	{
		auto l = GetStoreLabel(type);
		auto bytes = _binLib.Read(l);
		// call func_wrapper function
		// load byte and call func

		// return result
		return JsonObject(1.0);
	}

	// keyword maybe part package name, 需要去匹配，所以返回值可能不能做到返回函数的相关信息
	vector<FuncType> FunctionLibrary::Search(string const &keyword)
	{
		return {};
	}

	pos_label FunctionLibrary::GetStoreLabel(FuncType const& type)
	{
		if (_funcInfoCache.contains(type))
		{
			return _funcInfoCache[type];
		}

		if (_index.Contains(type))
		{
			auto l = _index.GetStoreLabel(type);
			_funcInfoCache.insert({ type, l });
			return l;
		}

		throw InvalidOperationException("Function not exist");
	}
}

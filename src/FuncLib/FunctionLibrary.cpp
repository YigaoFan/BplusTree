#include "FunctionLibrary.hpp"
#include "../Basic/Exception.hpp"
#include "Compile/CompileProcess.hpp"
#include "Compile/Invoke.hpp"

namespace FuncLib
{
	using Basic::InvalidOperationException;
	using ::std::move;
	using ::std::filesystem::is_directory;

	FunctionLibrary::FunctionLibrary(decltype(_index) index, decltype(_binLib) binLib)
		: _index(move(index)), _binLib(move(binLib))
	{
	}

	FunctionLibrary FunctionLibrary::GetFrom(path dirPath)
	{
		if (not is_directory(dirPath))
		{
			throw InvalidOperationException(string("Cannot operate on path: ") + dirPath.c_str() + " which is not directory");
		}

		auto indexFilePath = dirPath / "func.idx";
		auto binFilePath = dirPath / "func_bin.lib";
		auto i = FuncBinaryLibIndex::GetFrom(indexFilePath);
		auto b = FuncBinaryLib::GetFrom(binFilePath);
		return FunctionLibrary(move(i), move(b));
	}

	void FunctionLibrary::Add(vector<string> packageHierarchy, FuncDefTokenReader defReader, string summary)
	{
		auto [funcs, bin] = Compile::Compile(&defReader);
		auto p = _binLib.Add(move(bin));

		for (auto& f : funcs)
		{
			f.Type.PackageHierarchy = packageHierarchy;
			if (_index.Contains(f.Type))
			{
				throw InvalidOperationException("Function already exist: " + f.Type.ToString());
			}
			else
			{
				f.Summary = summary;
				// 使用 FuncObj 可以生成客户端调用代码
				_binLib.AddRefCount(p);
				_index.Add(f, p.Label());
			}
		}
	}

	bool FunctionLibrary::Contains(FuncType const& func) const
	{
		return _index.Contains(func);
	}

#define FUNC_NOT_EXIST_EXCEPTION(FUNC_TYPE) throw InvalidOperationException("Function not exist: " + FUNC_TYPE.ToString())
	// void FunctionLibrary::ModifyFuncName(FuncType const& func, string newFuncName)
	// {
		// 调用时的名字变了，要怎么处理
	// 	if (_index.Contains(func))
	// 	{
	// 		return _index.ModifyFuncName(func, move(newFuncName));
	// 	}

	// 	FUNC_NOT_EXIST_EXCEPTION(func);
	// }

	void FunctionLibrary::ModifyPackageOf(FuncType const &func, vector<string> packageHierarchy)
	{
		if (_index.Contains(func))
		{
			return _index.ModifyPackageOf(func, move(packageHierarchy));
		}

		FUNC_NOT_EXIST_EXCEPTION(func);
	}

	void FunctionLibrary::Remove(FuncType const& func)
	{
		if (_index.Contains(func))
		{
			auto l = GetStoreLabel(func);
			_binLib.DecreaseRefCount(l);
			_index.Remove(func);
			return;
		}

		FUNC_NOT_EXIST_EXCEPTION(func);
	}

	JsonObject FunctionLibrary::Invoke(FuncType const& func, JsonObject args)
	{
		// 改名字再调用会出现问题
		auto l = GetStoreLabel(func);
		auto bytes = _binLib.Read(l);
		auto wrapperFuncName = func.FuncName + "_wrapper";
		return Compile::Invoke(bytes, wrapperFuncName.c_str(), move(args));
	}

	// keyword maybe part package name, 需要去匹配，所以返回值可能不能做到返回函数的相关信息
	Generator<pair<string, string>> FunctionLibrary::Search(string const& keyword)
	{
		return _index.Search(keyword);
	}

	pos_label FunctionLibrary::GetStoreLabel(FuncType const& func)
	{
		if (_funcInfoCache.contains(func))
		{
			return _funcInfoCache[func];
		}

		if (_index.Contains(func))
		{
			auto l = _index.GetStoreLabel(func);
			_funcInfoCache.insert({ func, l });
			return l;
		}

		FUNC_NOT_EXIST_EXCEPTION(func);
	}
#undef FUNC_NOT_EXIST_EXCEPTION
}

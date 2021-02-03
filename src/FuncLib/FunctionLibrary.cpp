#include "FunctionLibrary.hpp"
#include "../Basic/Exception.hpp"
#include "Compile/CompileProcess.hpp"

namespace FuncLib
{
	using Basic::InvalidOperationException;
	using Compile::GetWrapperFuncName;
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

	void FunctionLibrary::Add(vector<string> package, FuncsDefReader defReader, string summary)
	{
		auto [funcs, bin] = Compile::Compile(move(defReader));
		auto p = _binLib.Add(move(bin));

		for (auto& f : funcs)
		{
			f.Type.Package = package;
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
		return _funcInfoCache.contains(func) or _index.Contains(func);
	}

#define FUNC_NOT_EXIST_EXCEPTION(FUNC_TYPE) throw InvalidOperationException("Function not exist: " + FUNC_TYPE.ToString())

	void FunctionLibrary::ModifyPackageOf(FuncType const& func, vector<string> package)
	{
		if (_funcInfoCache.contains(func) or _index.Contains(func))
		{
			if (_funcInfoCache.contains(func))
			{
				auto item = _funcInfoCache.extract(func);
				item.key().Package = package;
				_funcInfoCache.insert(move(item));
			}

			return _index.ModifyPackageOf(func, move(package));
		}


		FUNC_NOT_EXIST_EXCEPTION(func);
	}

	void FunctionLibrary::Remove(FuncType const& func)
	{
		if (_index.Contains(func))
		{
			auto l = GetStoreLabel(func);
			_binLib.DecreaseRefCount(l);
			if (_funcInfoCache.contains(func))
			{
				_funcInfoCache.erase(func);
			}
			_index.Remove(func);
			return;
		}

		FUNC_NOT_EXIST_EXCEPTION(func);
	}

	JsonObject FunctionLibrary::Invoke(FuncType const& func, JsonObject args)
	{
		auto l = GetStoreLabel(func);
		auto libPtr = _binLib.Load(l);
		auto wrapperFuncName = GetWrapperFuncName(func.FuncName);
		return libPtr->Invoke<InvokeFuncType>(wrapperFuncName.c_str(), move(args));
	}

	Generator<pair<string, string>> FunctionLibrary::Search(string const& keyword) const
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

	Generator<FuncType> FunctionLibrary::FuncTypes() const
	{
		return _index.FuncTypes();
	}
}

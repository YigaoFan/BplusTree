#include "FuncBinaryLib.hpp"

namespace FuncLib
{
	FuncBinaryLib::FuncBinaryLib(decltype(_file) file) : _file(move(file))
	{ }

	FuncBinaryLib FuncBinaryLib::GetFrom(path const& path)
	{
		auto f = File::GetFile(path);
		return FuncBinaryLib(move(f));
	}

	void FuncBinaryLib::DecreaseRefCount(pos_label label)
	{
		auto binUnitObj = _file->Read<BinUnit>(label);
		auto& refCount = binUnitObj->RefCount;
		--refCount;
		if (refCount == 0)
		{
			if (_cache.contains(label))
			{
				_cache.erase(label);
			}
			_file->Delete(label, binUnitObj);
		}
	}

	shared_ptr<SharedLibWithCleaner> FuncBinaryLib::Load(pos_label label)
	{
		if (_cache.contains(label))
		{
			return _cache[label];
		}

		auto binPtr = ReadBin(label);
		string tempFileName = "temp_invoke" + RandomString() + ".so";
		{
			ofstream of(tempFileName);
			of.write(binPtr->data(), binPtr->size());
		}
		auto lib = make_shared<SharedLibWithCleaner>(move(tempFileName));
		_cache.insert({ label, lib });
		return lib;
	}

	vector<char>* FuncBinaryLib::ReadBin(pos_label label)
	{
		return &_file->Read<BinUnit>(label)->Bin;
	}
}

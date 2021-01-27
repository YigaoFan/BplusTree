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
			_file->Delete(label, binUnitObj);
		}
	}

	vector<char>* FuncBinaryLib::Read(pos_label label)
	{
		return &_file->Read<BinUnit>(label)->Bin;
	}
}

#pragma once
#include <vector>
#include <memory>
#include <filesystem>
#include "Store/StaticConfig.hpp"
#include "Store/File.hpp"

namespace FuncLib
{
	using FuncLib::Store::File;
	using FuncLib::Store::pos_label;
	using ::std::move;
	using ::std::shared_ptr;
	using ::std::vector;
	using ::std::filesystem::path;

	struct BinUnit
	{
		int RefCount;
		vector<char> Bin;
	};

	template <typename Callback>
	class BookingPos
	{
	private:
		friend class FuncBinaryLib;
		pos_label _label;
		shared_ptr<BinUnit> _binUnitObj;
		Callback _callbackOnDestroy;

	public:
		BookingPos(pos_label label, shared_ptr<BinUnit> binObj, Callback callbackOnDestroy)
			: _label(label), _binUnitObj(move(binObj)), _callbackOnDestroy(move(callbackOnDestroy))
		{ }

		BookingPos(BookingPos const& that) = delete;

		pos_label Label() const
		{
			return _label;
		}

		~BookingPos()
		{
			_callbackOnDestroy(this);
		}
	};

	class FuncBinaryLib
	{
	private:
		
		shared_ptr<File> _file;

		FuncBinaryLib(decltype(_file) file) : _file(move(file))
		{ }

	public:
		static FuncBinaryLib GetFrom(path const& path)
		{
			// if exist, load
			// or create
			auto f = File::GetFile(path);
			return FuncBinaryLib(move(f));
		}
		
		auto Add(vector<char> bin)
		{
			auto [l, binUnitObj] = _file->New<BinUnit>(BinUnit{ 0, move(bin) });
			return BookingPos(l, binUnitObj, [&](auto* pos)
			{
				// 这样搞就不允许多线程添加了
				if (pos->_binUnitObj->RefCount != 0)
				{
					// 存的作用现在是，想清楚，是不是现在都要手动存？
					_file->Store(pos->Label(), pos->_binUnitObj);
				}
			});
		}

		template <typename T>
		void AddRefCount(BookingPos<T>& pos)
		{
			++pos._binUnitObj->RefCount;
		}

		void DecreaseRefCount(pos_label label)
		{
			auto binUnitObj = _file->Read<BinUnit>(label);
			auto &refCount = binUnitObj->RefCount;
			--refCount;
			if (refCount == 0)
			{
				_file->Delete(label, binUnitObj);
			}
		}

		vector<char> Read(pos_label label)
		{
			return _file->Read<BinUnit>(label)->Bin;
		}
	};
}

#pragma once
#include <vector>
#include <memory>
#include <filesystem>
#include <unordered_map>
#include "Store/StaticConfig.hpp"
#include "Persistence/ByteConverter.hpp"
#include "Store/File.hpp"

// temp
#include <string>
#include "Compile/SharedLibrary.hpp"
#include "Compile/Util.hpp"
#include <fstream>

namespace FuncLib
{
	using FuncLib::Store::File;
	using FuncLib::Store::pos_label;
	using ::std::move;
	using ::std::shared_ptr;
	using ::std::unordered_map;
	using ::std::vector;
	using ::std::filesystem::path;

	// temp
	using ::std::string;
	using FuncLib::Compile::SharedLibrary;
	using Compile::FilesCleaner;
	using ::std::ofstream;
	using Compile::RandomString;
	using ::std::make_shared;

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

		BookingPos(BookingPos&& that) noexcept = default;
		BookingPos(BookingPos const& that) = delete;
		pos_label Label() const
		{
			return _label;
		}

		~BookingPos()
		{
			if (_binUnitObj != nullptr)
			{
				_callbackOnDestroy(this);
			}
		}
	};

	class SharedLibWithCleaner;

	class FuncBinaryLib
	{
	private:
		shared_ptr<File> _file;
		unordered_map<pos_label, shared_ptr<SharedLibWithCleaner>> _cache;

		FuncBinaryLib(decltype(_file) file);
		
	public:
		static FuncBinaryLib GetFrom(path const& path);
		void DecreaseRefCount(pos_label label);
		shared_ptr<SharedLibWithCleaner> Load(pos_label label);
		vector<char>* ReadBin(pos_label label);

		auto Add(vector<char> bin)
		{
			auto [l, binUnitObj] = _file->New<BinUnit>(BinUnit{ 0, move(bin) });
			return BookingPos(l, binUnitObj, [&](auto* pos)
			{
				// 这样搞就不允许多线程添加了
				if (pos->_binUnitObj->RefCount != 0)
				{
					_file->Store(pos->Label(), pos->_binUnitObj);
				}
				else
				{
					_file->Delete(pos->Label(), pos->_binUnitObj);
				}
			});
		}

		template <typename T>
		void AddRefCount(BookingPos<T>& pos)
		{
			++pos._binUnitObj->RefCount;
		}
	};

	class SharedLibWithCleaner : private SharedLibrary, private FilesCleaner
	{
	private:
		using Base1 = SharedLibrary;
		using Base2 = FilesCleaner;

	public:
		SharedLibWithCleaner(string filename) : Base1(filename), Base2(filename)
		{
		}

		using SharedLibrary::Invoke;
	};
}

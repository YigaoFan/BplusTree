#pragma once
#include <string>
#include <memory>
#include <filesystem>
#include "Store/StaticConfig.hpp"
#include "Compile/FuncObj.hpp"
#include "Compile/FuncType.hpp"
#include "Store/File.hpp"
#include "Persistence/ByteConverter.hpp"
#include "Persistence/TypeConverter.hpp" // 这是因为 TypeSelector 里面用到了 TypeConverter，不优雅，想办法解决下
#include "../Btree/Btree.hpp"

namespace FuncLib
{
	using Collections::Btree;
	using Collections::StorePlace;
	using FuncLib::Compile::FuncObj;
	using FuncLib::Compile::FuncType;
	using FuncLib::Persistence::OwnerState;
	using FuncLib::Persistence::TypeConverter;
	using FuncLib::Persistence::UniqueDiskPtr;
	using FuncLib::Store::File;
	using FuncLib::Store::pos_label;
	using ::std::shared_ptr;
	using ::std::string;
	using ::std::filesystem::path;

	constexpr Collections::order_int Order = 3; // TODO

	class FuncBinaryLibIndex
	{
	private:
		using Key = string;
		using DiskBtree = Btree<Order, Key, pos_label, StorePlace::Disk>;
		shared_ptr<File> _file;
		shared_ptr<DiskBtree> _diskBtree;

		FuncBinaryLibIndex(shared_ptr<File> file, shared_ptr<DiskBtree> diskBtree)
			: _file(move(file)), _diskBtree(move(diskBtree))
		{ }

	public:
		static FuncBinaryLibIndex GetFrom(path const& path)
		{
			// if exist, load
			// or create
			constexpr pos_label btreeLabel = 1;// 这里是 hardcode 的
			auto f = File::GetFile(path);
			auto t = f->Read<DiskBtree>(btreeLabel);// 其实这里需要判断这个 label 存不存在的，不存在的话要新建
			return FuncBinaryLibIndex(move(f), move(t));
		}

#define STR_TO_DISK_REF_STR(STR) TypeConverter<string, OwnerState::FullOwner>::ConvertFrom(STR, _file.get())
		void Add(pos_label label, FuncType const& type)
		{
			_diskBtree->Add({ STR_TO_DISK_REF_STR(type.ToKey()), label });
		}

		pos_label GetStoreLabel(FuncType const& type) const
		{
			return _diskBtree->GetValue(type.ToKey());
		}

		bool Contains(FuncType const& type) const
		{
			return _diskBtree->ContainsKey(type.ToKey());
		}

		void ModifyFuncName(FuncType type, string newFuncName)
		{
			auto l = _diskBtree->GetValue(type.ToKey());
			_diskBtree->Remove(type.ToKey());

			type.FuncName(move(newFuncName));
			_diskBtree->Add({ STR_TO_DISK_REF_STR(type.ToKey()), l });
		}

		void ModifyPackageNameOf(FuncType type, vector<string> packageHierarchy)
		{
			auto l = _diskBtree->GetValue(type.ToKey());// 要不要直接传 key 进来？
			_diskBtree->Remove(type.ToKey());

			type.PackageHierarchy(move(packageHierarchy));
			_diskBtree->Add({ STR_TO_DISK_REF_STR(type.ToKey()), l });
		}

		void Remove(FuncType const& type)
		{
			_diskBtree->Remove(type.ToKey());
		}
	};
}

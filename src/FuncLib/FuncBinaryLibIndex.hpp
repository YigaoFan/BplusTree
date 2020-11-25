#pragma once
#include <string>
#include <memory>
#include <cassert>
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
	using ::std::filesystem::exists;
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
			auto firstSetup = not exists(path);
			auto file = File::GetFile(path);

			shared_ptr<DiskBtree> tree;
			auto pred = [](string const& s1, string const& s2)
			{
				return s1 < s2;
			};

			if (firstSetup)
			{
				auto [l, tree] = file->New(DiskBtree(move(pred)));
				assert(l == 1);// l should be 1
			}
			else
			{
				constexpr pos_label btreeLabel = 1; // 这里是 hardcode 为 1
				tree = file->Read<DiskBtree>(btreeLabel);
				tree->LessThanPredicate(move(pred));
			}
			
			return FuncBinaryLibIndex(move(file), move(tree));
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
			ModifyType(move(type), [n=move(newFuncName)](FuncType* oldType)
			{
				oldType->FuncName(move(n));
			});
		}

		void ModifyPackageNameOf(FuncType type, vector<string> packageHierarchy)
		{
			ModifyType(move(type), [h=move(packageHierarchy)](FuncType* oldType)
			{
				oldType->PackageHierarchy(move(h));
			});
		}

		void Remove(FuncType const& type)
		{
			_diskBtree->Remove(type.ToKey());
		}

	private:
		template <typename Callback>
		void ModifyType(FuncType oldType, Callback setTypeCallback)
		{
			auto l = _diskBtree->GetValue(oldType.ToKey());
			_diskBtree->Remove(oldType.ToKey());

			setTypeCallback(&oldType);
			_diskBtree->Add({ STR_TO_DISK_REF_STR(oldType.ToKey()), l });
		}
#undef STR_TO_DISK_REF_STR
	};
}

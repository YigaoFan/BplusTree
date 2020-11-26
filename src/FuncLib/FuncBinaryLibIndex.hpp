#pragma once
#include <string>
#include <memory>
#include <filesystem>
#include <vector>
#include "Store/StaticConfig.hpp"
#include "Compile/FuncType.hpp"
#include "Store/File.hpp"
#include "Persistence/ByteConverter.hpp"
#include "Persistence/TypeConverter.hpp" // 这是因为 TypeSelector 里面用到了 TypeConverter，不优雅，想办法解决下
#include "../Btree/Btree.hpp"

namespace FuncLib
{
	using Collections::Btree;
	using Collections::StorePlace;
	using FuncLib::Compile::FuncType;
	using FuncLib::Persistence::ComputeNodeMaxN;
	using FuncLib::Persistence::OwnerState;
	using FuncLib::Persistence::TypeConverter;
	using FuncLib::Store::File;
	using FuncLib::Store::pos_label;
	using ::std::shared_ptr;
	using ::std::string;
	using ::std::vector;
	using ::std::filesystem::path;


	class FuncBinaryLibIndex
	{
	private:
		// static constexpr Collections::order_int Order = ComputeNodeMaxN<string, pos_label>();
		static constexpr Collections::order_int Order = 3;

		using Key = string;
		using DiskBtree = Btree<Order, Key, pos_label, StorePlace::Disk>;
		shared_ptr<File> _file;
		shared_ptr<DiskBtree> _diskBtree;

		FuncBinaryLibIndex(shared_ptr<File> file, shared_ptr<DiskBtree> diskBtree);

	public:
		static FuncBinaryLibIndex GetFrom(path const& path);
		void Add(pos_label label, FuncType const& type);
		pos_label GetStoreLabel(FuncType const& type) const;
		bool Contains(FuncType const& type) const;
		void ModifyFuncName(FuncType type, string newFuncName);
		void ModifyPackageOf(FuncType type, vector<string> packageHierarchy);
		void Remove(FuncType const& type);

	private:
		template <typename Callback>
		void ModifyType(FuncType oldType, Callback setTypeCallback);
	};
}

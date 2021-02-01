#pragma once
#include <string>
#include <memory>
#include <filesystem>
#include <vector>
#include <utility>
#include "Store/StaticConfig.hpp"
#include "Compile/FuncObj.hpp"
#include "Store/File.hpp"
// 这是因为 Btree 里 TypeSelector 里面用到了 TypeConverter
#include "Persistence/TypeConverter.hpp"
#include "../Btree/Btree.hpp"
#include "../Btree/Generator.hpp"

namespace FuncLib
{
	using Collections::Btree;
	using Collections::Generator;
	using Collections::StorePlace;
	using FuncLib::Compile::FuncObj;
	using FuncLib::Compile::FuncType;
	using FuncLib::Persistence::ComputeNodeMaxN;
	using FuncLib::Persistence::OwnerState;
	using FuncLib::Persistence::TypeConverter;
	using FuncLib::Store::File;
	using FuncLib::Store::pos_label;
	using ::std::pair;
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
		using DiskBtree = Btree<Order, Key, pair<pos_label, pair<string, string>>, StorePlace::Disk>;
		shared_ptr<File> _file;
		shared_ptr<DiskBtree> _diskBtree;

		FuncBinaryLibIndex(shared_ptr<File> file, shared_ptr<DiskBtree> diskBtree);

	public:
		static FuncBinaryLibIndex GetFrom(path const& path);
		FuncBinaryLibIndex(FuncBinaryLibIndex&& that) noexcept = default;
		FuncBinaryLibIndex(FuncBinaryLibIndex const& that) = delete;
		void Add(FuncObj const& funcObj, pos_label label);
		pos_label GetStoreLabel(FuncType const& type) const;
		bool Contains(FuncType const& type) const;
		void ModifyFuncName(FuncType type, string newFuncName);
		void ModifyPackageOf(FuncType type, vector<string> packageHierarchy);
		void Remove(FuncType const& type);
		/// pair: Key, summary
		Generator<pair<string, string>> Search(string const& keyword) const;
		Generator<FuncType> FuncTypes() const;
		~FuncBinaryLibIndex();

	private:
		template <typename Callback>
		void ModifyType(FuncType oldType, Callback setTypeCallback);
	};
}

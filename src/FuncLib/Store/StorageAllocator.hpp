#pragma once
#include <set>
#include <utility>
#include <filesystem>
#include <map>
#include "StaticConfig.hpp"
#include "ObjectBytes.hpp"
#include "../Persistence/FriendFuncLibDeclare.hpp"
#include "../Persistence/IWriterIReaderConcept.hpp"

namespace FuncLib::Store
{
	using Persistence::IReader;
	using ::std::map;
	using ::std::pair;
	using ::std::set;
	using ::std::filesystem::path;

	class StorageAllocator
	{
	private:
		friend struct Persistence::ByteConverter<StorageAllocator, false>;
		friend StorageAllocator ReadAllocatedInfoFrom(IReader auto* reader);
		friend void WriteAllocatedInfoTo(StorageAllocator const& allocator, ObjectBytes* bytes);
		pos_int _currentPos;
		// 实际上这里相当于是偏移，最后在 OutDiskPtr 里面可以加一个基础地址
		// 分配的也是偏移
		map<pos_label, pair<pos_int, size_t>> _usingLabelTable;
		map<pos_label, pair<pos_int, size_t>> _deletedLabelTable; // 优先从这里分配
	public:
		StorageAllocator() = default;

		pos_label AllocatePosLabel();
		bool Ready(pos_label posLabel) const;
		/// below for first use
		pos_int GiveSpaceTo(pos_label posLabel, size_t size);
		/// 下面两个函数都是在 GiveSpaceTo 做好预备后调用，否则无效
		pos_int GetConcretePos(pos_label posLabel) const;
		size_t GetAllocatedSize(pos_label posLabel) const;
		pos_int ResizeSpaceTo(pos_label posLabel, size_t biggerSize);
		void DeallocatePosLabel(pos_label posLabel);
		void DeallocatePosLabels(set<pos_label> const& posLabels);
	private:
		StorageAllocator(pos_int currentPos, map<pos_label, pair<pos_int, size_t>> posLabelTable, map<pos_label, pair<pos_int, size_t>> deletedLabels);
	};
}
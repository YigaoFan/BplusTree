#pragma once
#include <set>
#include <utility>
#include <filesystem>
#include <map>
#include "StaticConfig.hpp"

namespace FuncLib::Store
{
	using ::std::map;
	using ::std::pair;
	using ::std::set;
	using ::std::filesystem::path;

	// 分配信息需要保存到硬盘上
	class StorageAllocator
	{
	private:
		pos_int _currentPos;
		// 实际上这里相当于是偏移，最后在 OutDiskPtr 里面可以加一个基础地址
		// 分配的也是偏移
		map<pos_lable, pair<pos_int, size_t>> _posLableTable;
		map<pos_lable, pair<pos_int, size_t>> _deletedLables; // 优先从这里分配
		StorageAllocator(pos_int currentPos, map<pos_lable, pair<pos_int, size_t>> ownerTable);
	public:
		static StorageAllocator ReadAllocatedInfoFrom(path const& filename);
		static void WriteAllocatedInfoTo(path const& filename, StorageAllocator const& allocator);

		pos_lable AllocatePosLable();
		bool Ready(pos_lable posLable) const;
		/// below for first use
		pos_int GiveSpaceTo(pos_lable posLable, size_t size);
		/// 下面两个函数都是在 GiveSpaceTo 做好预备后调用，否则无效
		pos_int GetConcretePos(pos_lable posLable) const;
		size_t GetAllocatedSize(pos_lable posLable) const;
		pos_int ResizeSpaceTo(pos_lable posLable, size_t biggerSize);
		void DeallocatePosLable(pos_lable posLable);
		void DeallocatePosLables(set<pos_lable> const& posLables);
	};
}
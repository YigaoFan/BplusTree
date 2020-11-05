#pragma once
#include <set>
#include <utility>
#include <filesystem>
#include <map>
#include "StaticConfig.hpp"
#include "FileReader.hpp"
#include "ObjectBytes.hpp"
#include "../Persistence/FriendFuncLibDeclare.hpp"

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
		friend struct Persistence::ByteConverter<StorageAllocator, false>;
		pos_int _currentPos;
		// 实际上这里相当于是偏移，最后在 OutDiskPtr 里面可以加一个基础地址
		// 分配的也是偏移
		map<pos_lable, pair<pos_int, size_t>> _usingLableTable;
		map<pos_lable, pair<pos_int, size_t>> _deletedLableTable; // 优先从这里分配
	public:
		static StorageAllocator ReadAllocatedInfoFrom(FileReader* reader);
		static void WriteAllocatedInfoTo(StorageAllocator const& allocator, ObjectBytes* bytes);

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
	private:
		StorageAllocator(pos_int currentPos, map<pos_lable, pair<pos_int, size_t>> posLableTable, map<pos_lable, pair<pos_int, size_t>> deletedLables);
	};
}
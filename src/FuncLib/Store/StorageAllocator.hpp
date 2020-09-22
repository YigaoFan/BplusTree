#pragma once
#include <memory>
#include <vector>
#include <filesystem>
#include "StaticConfig.hpp"
#include "InsidePositionOwner.hpp"

namespace FuncLib::Store
{
	using ::std::shared_ptr;
	using ::std::vector;
	using ::std::filesystem::path;

	// 分配信息需要保存到硬盘上
	class StorageAllocator
	{
	private:
		pos_int _currentPos;
		// 实际上这里相当于是偏移，最后在 OutDiskPtr 里面可以加一个基础地址
		// 分配的也是偏移
		vector<shared_ptr<InsidePositionOwner>> _ownerTable; // InsidePositionOwner 这个类应该是要让存储的位置信息的时候要用
		StorageAllocator(pos_int currentPos, vector<shared_ptr<InsidePositionOwner>> ownerTable);
	public:
		static StorageAllocator ReadAllocatedInfoFrom(path const& filename);
		void SignIn(shared_ptr<InsidePositionOwner> positionOwner);
		pos_int Allocate(size_t size);
		pos_int Reallocate(pos_int previousPosition, size_t newSize);
	};
}
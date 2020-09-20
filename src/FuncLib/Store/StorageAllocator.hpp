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
	
	class File;
	// 分配信息需要保存到硬盘上
	class StorageAllocator
	{
	private:
		File* _file;
		pos_int _currentPos;
		vector<shared_ptr<InsidePositionOwner>> _ownerTable; // InsidePositionOwner 这个类应该是要让存储的位置信息的时候要用
	public:
		StorageAllocator(File* file);
		void SignIn(shared_ptr<InsidePositionOwner> positionOwner);
		pos_int Allocate(size_t size);
		pos_int Reallocate(pos_int previousPosition, size_t newSize);
	private:
		static vector<shared_ptr<InsidePositionOwner>> ReadAllocatedInfoFrom(File const* file);
	};
}
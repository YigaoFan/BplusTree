#pragma once
#include <memory>
#include <vector>
#include <filesystem>
#include "StaticConfig.hpp"
#include "IInsidePositionOwner.hpp"

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
		vector<shared_ptr<IInsidePositionOwner>> _ownerTable;
	public:
		StorageAllocator(File* file);
		void SignIn(shared_ptr<IInsidePositionOwner> positionOwner);

		template <typename T>
		shared_ptr<IInsidePositionOwner> Allocate()
		{
			auto pos = _currentPos;
			_currentPos += sizeof(T);
			auto owner = MakePositionOwner<T>(_file, pos);
			_ownerTable.emplace_back(owner);
			return owner;
		}

	private:
		static vector<shared_ptr<IInsidePositionOwner>> ReadAllocatedInfoFrom(File const* file);
	};
}
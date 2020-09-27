#pragma once
#include <memory>
#include <vector>
#include <utility>
#include <filesystem>
#include <map>
#include "StaticConfig.hpp"
#include "InsidePositionOwner.hpp"

namespace FuncLib::Store
{
	using ::std::map;
	using ::std::pair;
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
		// vector<shared_ptr<InsidePositionOwner>> _ownerTable; // InsidePositionOwner 这个类应该是要让存储的位置信息的时候要用
		map<pos_lable, pair<pos_int, size_t>> _posLableTable;
		StorageAllocator(pos_int currentPos, map<pos_lable, pair<pos_int, size_t>> ownerTable);
		// 这里加一成员放已删除的 pos_lable 的集合，用于 GetConcretePos 的防御编程
	public:
		static StorageAllocator ReadAllocatedInfoFrom(path const& filename);
		static void WriteAllocatedInfoTo(path const& filename)
		{
			// 使用 ByteConverter<vector<T>, false> 来持久化这里的信息
		}

		bool Ready(pos_lable posLable) const
		{
			return _posLableTable.contains(posLable);
		}

		pos_int GetConcretePos(pos_lable posLable) const
		{
			if (_posLableTable.contains(posLable))
			{
				return _posLableTable[posLable].first;
			}
			else
			{
				// allocate now
			}
			// 这里可以防御编程一下，如果有的 pos_lable 刚好删了，那就报错
		}

		size_t GetAllocatedSize(pos_lable posLable) const
		{
			if (_posLableTable.contains(posLable))
			{
				return _posLableTable[posLable].second;
			}
			else
			{
				// allocate now
			}
			// 这里可以防御编程一下，如果有的 pos_lable 刚好删了，那就报错
		}

		pos_lable AllocatePosLable()
		{
			return _posLableTable.size();
			// _posLableTable.insert({ lable, { 0, 0 }});// { 0, 0 } means wait for future concrete allocate
			// 是不是应该把这个 value 的 pair share 出去？
			// share 到 InsidePosition 里面，DiskPos 首次需要到 alloc 那里读一下设置到 InsidePosition 里
			// 之后由于 share，就可以自动获取到最新的位置了
			// DiskPos 可以不继承 InsidePosition ，可以把它组合进去
			// 读写 object 的地方都要考虑位置的应用
		}

		void DeallocatePosLable(pos_lable posLable)
		{
			_posLableTable.erase(posLable);// 那是什么时候调整分配大小，那时候调用上面具体位置的地方就会受影响
		}


		/// for first use
		pos_int GiveSpaceTo(pos_lable posLable)
		{

		}
		
		pos_int ResizeSpaceTo(pos_lable posLable, size_t biggerSize);
	};
}
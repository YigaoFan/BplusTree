#include "../../Basic/Exception.hpp"
#include "StorageAllocator.hpp"
#include "../Persistence/ByteConverter.hpp"

/// 其他类支持 ByteConverter 的样板
namespace FuncLib::Persistence
{
	using FuncLib::Store::StorageAllocator;

	template <>
	struct ByteConverter<StorageAllocator, false>
	{
		using ThisType = StorageAllocator;
		using DataMember0 = decltype(declval<ThisType>()._currentPos);
		using DataMember1 = decltype(declval<ThisType>()._usingLableTable);
		using DataMember2 = decltype(declval<ThisType>()._deletedLableTable);
		static constexpr bool SizeStable = All<GetSizeStable, DataMember0, DataMember1, DataMember2>::Result;

		static void WriteDown(ThisType const& p, IWriter auto* writer)
		{
			ByteConverter<DataMember0>::WriteDown(p._currentPos, writer);
			ByteConverter<DataMember1>::WriteDown(p._usingLableTable, writer);
			ByteConverter<DataMember2>::WriteDown(p._deletedLableTable, writer);
		}

		static ThisType ReadOut(FileReader* reader)
		{
			return
			{
				ByteConverter<DataMember0>::ReadOut(reader),
				ByteConverter<DataMember1>::ReadOut(reader),
				ByteConverter<DataMember2>::ReadOut(reader)
			};
		}
	};
}

namespace FuncLib::Store
{
	StorageAllocator StorageAllocator::ReadAllocatedInfoFrom(FileReader* reader)
	{
		return ByteConverter<StorageAllocator>::ReadOut(reader);
	}

	void StorageAllocator::WriteAllocatedInfoTo(StorageAllocator const& allocator, ObjectBytes* bytes)
	{
		ByteConverter<StorageAllocator>::WriteDown(allocator, bytes);
	}

	StorageAllocator::StorageAllocator(pos_int currentPos, map<pos_lable, pair<pos_int, size_t>> usingLableTable, map<pos_lable, pair<pos_int, size_t>> deletedLableTable)
		: _currentPos(currentPos), _usingLableTable(move(usingLableTable)), _deletedLableTable(move(deletedLableTable))
	{ }

	bool StorageAllocator::Ready(pos_lable posLable) const
	{
		return _usingLableTable.contains(posLable);
	}

	pos_int StorageAllocator::GetConcretePos(pos_lable posLable) const
	{
		return _usingLableTable.find(posLable)->second.first;
	}

	size_t StorageAllocator::GetAllocatedSize(pos_lable posLable) const
	{
		return _usingLableTable.find(posLable)->second.second;
	}

	pos_lable StorageAllocator::AllocatePosLable()
	{
		/// 0 represents File, so from 1
		return _usingLableTable.size() + 1;
	}

	void StorageAllocator::DeallocatePosLable(pos_lable posLable)
	{
		if (_usingLableTable.contains(posLable))
		{
			auto allocateInfoIter = _usingLableTable.find(posLable);
			_deletedLableTable.insert(*allocateInfoIter);
			_usingLableTable.erase(posLable);
			// 那是什么时候调整分配大小，那时候调用上面具体位置的地方就会受影响，所以要尽量少的依赖具体位置
		}
	}

#define VALID_CHECK                                                                                     \
	if (_deletedLableTable.contains(posLable))                                                          \
	{                                                                                                   \
		using ::Basic::InvalidAccessException;                                                          \
		throw InvalidAccessException("Apply space for deleted lable, it means have wrong code logic."); \
	}
	/// for first use
	pos_int StorageAllocator::GiveSpaceTo(pos_lable posLable, size_t size)
	{
		VALID_CHECK;
		
		// 这里是个粗糙的内存分配算法实现
		_usingLableTable.insert({ posLable, { _currentPos, size }});
		_currentPos += size;
		return _currentPos - size;
	}

	pos_int StorageAllocator::ResizeSpaceTo(pos_lable posLable, size_t biggerSize)
	{
		VALID_CHECK;

		using ::std::make_pair;
		auto allocateInfoIter = _usingLableTable.find(posLable);
		_deletedLableTable.insert({ FileLable, allocateInfoIter->second });
		allocateInfoIter->second = make_pair(_currentPos, biggerSize);
		_currentPos += biggerSize;
		return _currentPos - biggerSize;
	}
#undef VALID_CHECK

	void StorageAllocator::DeallocatePosLables(set<pos_lable> const& posLables)
	{
		for (auto p : posLables)
		{
			DeallocatePosLable(p);
		}
	}
}

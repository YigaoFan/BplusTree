#include "../../Basic/Exception.hpp"
#include "StorageAllocator.hpp"

namespace FuncLib::Store
{
	StorageAllocator::StorageAllocator(pos_int currentPos, map<pos_label, pair<pos_int, size_t>> usingLabelTable, map<pos_label, pair<pos_int, size_t>> deletedLabelTable)
		: _currentPos(currentPos), _usingLabelTable(move(usingLabelTable)), _deletedLabelTable(move(deletedLabelTable))
	{ }

	bool StorageAllocator::Ready(pos_label posLabel) const
	{
		return _usingLabelTable.contains(posLabel);
	}

	pos_int StorageAllocator::GetConcretePos(pos_label posLabel) const
	{
		return _usingLabelTable.find(posLabel)->second.first;
	}

	size_t StorageAllocator::GetAllocatedSize(pos_label posLabel) const
	{
		return _usingLabelTable.find(posLabel)->second.second;
	}

	pos_label StorageAllocator::AllocatePosLabel()
	{
		/// 0 represents File, so from 1
		return _usingLabelTable.size() + 1;
	}

	void StorageAllocator::DeallocatePosLabel(pos_label posLabel)
	{
		if (_usingLabelTable.contains(posLabel))
		{
			auto allocateInfoIter = _usingLabelTable.find(posLabel);
			_deletedLabelTable.insert(*allocateInfoIter);
			_usingLabelTable.erase(posLabel);
			// 那是什么时候调整分配大小，那时候调用上面具体位置的地方就会受影响，所以要尽量少的依赖具体位置
		}
	}

#define VALID_CHECK                                                                                     \
	if (_deletedLabelTable.contains(posLabel))                                                          \
	{                                                                                                   \
		using ::Basic::InvalidAccessException;                                                          \
		throw InvalidAccessException("Apply space for deleted label, it means have wrong code logic."); \
	}
	/// for first use
	void StorageAllocator::GiveSpaceTo(pos_label posLabel, size_t size)
	{
		VALID_CHECK;
		
		// 这里是个粗糙的内存分配算法实现
		_usingLabelTable.insert({ posLabel, { _currentPos, size }});
		_currentPos += size;
	}

	void StorageAllocator::ResizeSpaceTo(pos_label posLabel, size_t biggerSize)
	{
		VALID_CHECK;

		using ::std::make_pair;
		auto allocateInfoIter = _usingLabelTable.find(posLabel);
		_deletedLabelTable.insert({ FileLabel, allocateInfoIter->second });
		allocateInfoIter->second = make_pair(_currentPos, biggerSize);
		_currentPos += biggerSize;
	}
#undef VALID_CHECK

	void StorageAllocator::DeallocatePosLabels(set<pos_label> const& posLabels)
	{
		for (auto p : posLabels)
		{
			DeallocatePosLabel(p);
		}
	}
}

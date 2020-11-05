#pragma once
#include <vector>

namespace FuncLib::Store
{
	using ::std::vector;

	class ObjectBytes;

	class ObjectBytesQueue
	{
	protected:
		vector<ObjectBytes*> _objBytesQueue;

	public:
		ObjectBytesQueue() = default;
		ObjectBytesQueue(vector<ObjectBytes*> objBytesQueue);
		void Add(ObjectBytes* objectBytes);

		auto begin() const { return _objBytesQueue.begin(); }
		auto end() const { return _objBytesQueue.end(); }
		auto begin() { return _objBytesQueue.begin(); }
		auto end() { return _objBytesQueue.end(); }

		template <typename Callback>
		ObjectBytesQueue& operator| (Callback callback)
		{
			for (auto x : (*this))
			{
				callback(x);
			}

			return *this;
		}
	};

	struct AllocateSpaceQueue : protected ObjectBytesQueue
	{
		using ObjectBytesQueue::Add;
		using ObjectBytesQueue::operator|;
		using ObjectBytesQueue::begin;
		using ObjectBytesQueue::end;
	};

	struct ResizeSpaceQueue : protected ObjectBytesQueue
	{
		using ObjectBytesQueue::Add;
		using ObjectBytesQueue::operator|;
		using ObjectBytesQueue::begin;
		using ObjectBytesQueue::end;
	};

	struct WriteQueue : protected ObjectBytesQueue
	{
		using ObjectBytesQueue::Add;
		using ObjectBytesQueue::operator|;
		using ObjectBytesQueue::begin;
		using ObjectBytesQueue::end;
	};
}

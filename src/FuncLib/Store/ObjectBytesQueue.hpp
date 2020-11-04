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
		ObjectBytesQueue(vector<ObjectBytes*> objBytesQueue) : _objBytesQueue(move(objBytesQueue))
		{ }

		void Add(ObjectBytes* objectBytes)
		{
			_objBytesQueue.push_back(objectBytes);
		}

		auto begin() const { return _objBytesQueue.begin(); }
		auto end() const { return _objBytesQueue.end(); }
		auto begin() { return _objBytesQueue.begin(); }
		auto end() { return _objBytesQueue.end(); }
	};

	struct AllocateSpaceQueue : protected ObjectBytesQueue
	{
		friend struct WriteQueue;
		using ObjectBytesQueue::Add;
		using ObjectBytesQueue::begin;
		using ObjectBytesQueue::end;
	};

	struct ResizeSpaceQueue : protected ObjectBytesQueue
	{
		friend struct WriteQueue;
		using ObjectBytesQueue::Add;
		using ObjectBytesQueue::begin;
		using ObjectBytesQueue::end;
	};

	struct WriteQueue : protected ObjectBytesQueue
	{
		WriteQueue() = default;
		WriteQueue(AllocateSpaceQueue&& allocatedQueue) noexcept
			: ObjectBytesQueue(move(allocatedQueue._objBytesQueue))
		{ }

		WriteQueue(ResizeSpaceQueue&& resizedQueue) noexcept
			: ObjectBytesQueue(move(resizedQueue._objBytesQueue))
		{ }

		using ObjectBytesQueue::Add;
		using ObjectBytesQueue::begin;
		using ObjectBytesQueue::end;
	};
}

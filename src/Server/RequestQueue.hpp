#include <mutex>
#include <queue>

namespace Server
{
	using ::std::lock_guard;
	using ::std::move;
	using ::std::mutex;
	using ::std::queue;

	template <typename T>
	class RequestQueue
	{
	private:
		queue<T> _queue;
		mutex _mutex;

	public:
		RequestQueue() {}

		RequestQueue(RequestQueue &&that) noexcept
			: _queue(MoveFrom(move(that))), _mutex()
		{ }

		T* Add(T t)
		{
			lock_guard<mutex> guard(_mutex);
			while (_queue.front().Done)
			{
				_queue.pop();
			}

			_queue.emplace(move(t));
			return &_queue.back();
		}

	private:
		static queue<T> MoveFrom(RequestQueue&& that)
		{
			lock_guard<mutex> guard(that._mutex);
			return move(that._queue);
		}
	};
}

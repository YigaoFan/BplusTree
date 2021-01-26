#include <mutex>
#include <queue>
#include <memory>

namespace Server
{
	using ::std::lock_guard;
	using ::std::make_shared;
	using ::std::move;
	using ::std::mutex;
	using ::std::queue;
	using ::std::shared_ptr;

	template <typename T>
	class RequestQueue
	{
	private:
		queue<shared_ptr<T>> _queue;
		mutex _mutex;

	public:
		RequestQueue() {}

		RequestQueue(RequestQueue&& that) noexcept
			: _queue(MoveFrom(move(that))), _mutex()
		{ }

		shared_ptr<T> Add(T t)
		{
			lock_guard<mutex> guard(_mutex);
			while (_queue.front()->Done)
			{
				_queue.pop();
			}

			_queue.emplace(make_shared<T>(move(t)));
			return _queue.back();
		}

	private:
		static queue<shared_ptr<T>> MoveFrom(RequestQueue&& that)
		{
			lock_guard<mutex> guard(that._mutex);
			return move(that._queue);
		}
	};
}

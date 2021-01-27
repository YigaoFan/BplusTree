#pragma once
#include <experimental/coroutine>

namespace Server
{
	using ::std::move;
	using ::std::experimental::coroutine_handle;
	using ::std::experimental::suspend_never;
	using ::std::experimental::suspend_always;

	class Task
	{
	public:
		struct promise_type
		{
			suspend_never initial_suspend()
			{
				return {};
			}

			suspend_always final_suspend() noexcept
			{
				return {};
			}

			void unhandled_exception()
			{
			}

			Task get_return_object()
			{
				using coro_handle = coroutine_handle<promise_type>;
				return Task(coro_handle::from_promise(*this));
			}

			void return_void() {}
		};

		using coro_handle = coroutine_handle<promise_type>;
		Task(coro_handle handle) : handle(move(handle)) { }

		Task(Task const& that) = delete;

		Task(Task&& that) noexcept : handle(move(that.handle))
		{
			that.handle = nullptr;
		}

		/// Maybe not thread safe
		bool IsCompleted() const
		{
			return handle.done();
		}

		void Wait()
		{
			while (not IsCompleted())
			{
				// memory barriers
				// inspired from: https://stackoverflow.com/questions/21217123/is-it-safe-to-poll-for-a-variable-written-by-other-threads-in-a-c-program
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}

		~Task()
		{
			if (handle != nullptr)
			{
				handle.destroy();
			}
		}
	private:
		coro_handle handle;
	};
}
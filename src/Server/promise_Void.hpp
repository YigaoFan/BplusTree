#pragma once
#include <experimental/coroutine>

namespace Server
{
	using ::std::experimental::coroutine_handle;
	using ::std::experimental::suspend_never;

	struct Void
	{
		struct promise_type
		{
			suspend_never initial_suspend()
			{
				return {};
			}

			suspend_never final_suspend() noexcept
			{
				return {};
			}

			void unhandled_exception()
			{
				// 在协程展开代码后，unhandled_exception 之后运行到的是 final_suspend
				// 而 final_suspend 后会销毁协程
				// using coro_handle = coroutine_handle<promise_type>;
				// auto handle = coro_handle::from_promise(*this);
				// handle.destroy();
			}

			Void get_return_object()
			{
				return {};
			}

			void return_void() {}
		};
	};
}
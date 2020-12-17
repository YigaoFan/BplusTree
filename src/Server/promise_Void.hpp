#pragma once
#include <experimental/coroutine>

namespace Server
{
	using ::std::experimental::suspend_never;

	struct Void
	{
		struct promise_type
		{
			suspend_never initial_suspend()
			{
				return {};
			}

			// 不 final_suspend 了，还需要显式 destroy handle 吗？
			suspend_never final_suspend() noexcept
			{
				return {};
			}

			// 让函数自己处理好异常
			void unhandled_exception()
			{
			}

			Void get_return_object()
			{
				return {};
			}

			void return_void() {}
		};
	};
}
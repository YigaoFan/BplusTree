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

			void unhandled_exception()
			{
				// std::terminate(); 这里可能要处理下
				// 要把异常给出去
			}

			Void get_return_object()
			{
				return {};
			}

			void return_void() {}
		};
	};
}
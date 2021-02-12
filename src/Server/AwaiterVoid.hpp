#pragma once
#include <utility>
#include <exception>
#include <experimental/coroutine>

namespace Server
{
	using ::std::exchange;
	using ::std::experimental::coroutine_handle;
	using ::std::experimental::suspend_never;
	using ::std::exception_ptr;

	struct Void
	{
		struct FinalAwaiter
		{
			bool await_ready() noexcept
			{
				return false;
			}

			// Final在外层还没有 Void 的情况下，就不用等了，直接进入 destroy 环节
			bool await_suspend(auto handle) noexcept
			{
				auto& p = handle.promise();
				if (auto c = p.Continuation; c != nullptr)
				{
					c.resume();
				}

				return false;
			}

			void await_resume() noexcept {}
		};

		struct promise_type
		{
			coroutine_handle<promise_type> Continuation = nullptr;
			exception_ptr ExceptionPtr = nullptr;

			suspend_never initial_suspend()
			{
				return {};
			}

			FinalAwaiter final_suspend() noexcept
			{
				return {};
			}

			void unhandled_exception()
			{
				// 在协程展开代码后，unhandled_exception 之后运行到的是 final_suspend
				// 而 final_suspend 后会销毁协程
				// 所以只需要在这里处理异常
				ExceptionPtr = std::current_exception();
			}

			Void get_return_object()
			{
				return coroutine_handle<promise_type>::from_promise(*this);
			}

			void return_void() {}
		};
		
		using coro_handle = coroutine_handle<promise_type>;

		bool await_ready() const noexcept
		{
			return false;
		}

		void await_suspend(coro_handle handle) const noexcept
		{
			this->handle.promise().Continuation = handle;
		}

		void await_resume() const noexcept
		{
			auto& p = handle.promise();
			if (p.ExceptionPtr != nullptr)
			{
				std::rethrow_exception(p.ExceptionPtr);
			}
		}

		Void(coro_handle handle) : handle(move(handle))
		{ }

		Void(Void&& that) noexcept : handle(exchange(that.handle, {}))
		{ }

		Void(Void const& that) = delete;

		~Void()
		{
			// handle 所指对象由发起 resume 的部分来销毁
		}
	private:
		coroutine_handle<promise_type> handle;
	};
}
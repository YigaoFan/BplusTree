#pragma once
#include "ThreadPool.hpp"
#include <condition_variable>
#include <exception>
#include <mutex>
#include <optional>
#include <utility>
#include <experimental/coroutine>

namespace Server
{
	using ::std::condition_variable;
	using ::std::exchange;
	using ::std::experimental::coroutine_handle;
	using ::std::experimental::suspend_never;
	using ::std::exception_ptr;
	using ::std::mutex;
	using ::std::optional;
	using ::std::unique_lock;
	using ::std::lock_guard;

	struct Void
	{
		struct promise_type
		{
			mutex ContinuationMutex;
			condition_variable CondVar;
			optional<coroutine_handle<promise_type>> Continuation;
			exception_ptr ExceptionPtr = nullptr;

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
					// 这里的异常处理还不完备，先这样
					if (p.ExceptionPtr != nullptr)
					{
						return true;
					}

					{
						unique_lock<mutex> lock(p.ContinuationMutex);
						p.CondVar.wait(lock, [&]
						{
							return p.Continuation.has_value();
						});
					}
					printf("continuation get value\n");
					if (auto c = p.Continuation.value(); c != nullptr)
					{
						printf("continuation resume\n");
						c.resume();
					}

					return false;
				}

				void await_resume() noexcept {}
			};

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
				printf("Exception registered\n");
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
			auto& p = this->handle.promise();
			{
				lock_guard<mutex> lock(p.ContinuationMutex);
				p.Continuation = handle;
			}
			p.CondVar.notify_one();
		}

		void await_resume() noexcept
		{
			auto& p = handle.promise();
			if (auto exception = p.ExceptionPtr; exception != nullptr)
			{
				handle.resume(); //这里是触发 destroy 释放内存
				// printf("exception rethrow\n");
				std::rethrow_exception(exception);
			}
		}

		Void(coro_handle handle) : handle(move(handle))
		{ }

		Void(Void&& that) noexcept : handle(exchange(that.handle, {}))
		{ }

		Void(Void const& that) = delete;

		~Void()
		{
			if (handle == nullptr)
			{
				return;
			}
			auto& p = handle.promise();
			if (not p.Continuation.has_value())
			{
				{
					// 上面直接就读了，可能也没有问题，因为设置 Continuation 的地方是顺序运行的
					lock_guard<mutex> lock(p.ContinuationMutex);
					p.Continuation = nullptr;
					printf("continuation has value %d\n", p.Continuation.has_value());
				}
				p.CondVar.notify_one();
			}
			// handle 所指对象由发起 resume 的部分来销毁
		}
	private:
		coroutine_handle<promise_type> handle;
	};
}
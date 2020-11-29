#pragma once
#include <utility>
#include <experimental/coroutine>

namespace Collections
{
	using ::std::exchange;
	using ::std::move;
	using ::std::experimental::coroutine_handle;
	using ::std::experimental::noop_coroutine;
	using ::std::experimental::suspend_always;

	template <typename T>
	struct Generator
	{
		friend struct promise_type;

		struct promise_type
		{
			using coro_handle = coroutine_handle<promise_type>;
			T value;

			T& Value()
			{
				return value;
			}

			suspend_always initial_suspend()
			{
				return {};
			}

			suspend_always final_suspend()
			{
				return {};
			}

			void unhandled_exception()
			{
				std::terminate();
			}

			auto yield_value(T t)
			{
				value = move(t);
				return suspend_always();
			}

			Generator get_return_object()
			{
				return coro_handle::from_promise(*this);
			}

			void return_void() {} // 为什么 return_void 可以，这个函数是干嘛用的
		};
		using coro_handle = coroutine_handle<promise_type>;

		bool MoveNext()
		{
			if (not handle.done())
			{
				handle.resume();
				return not handle.done();
			}

			return false;
		}

		T& Current()
		{
			return handle.promise().Value();
		}

		Generator(coro_handle handle) : handle(handle) {}

		~Generator()
		{
			// 因为 Generator 还没有复制的情况，所以不用判断 handle 是否有效
			handle.destroy();
		}

	private:
		coro_handle handle;
	};

	template <typename T>
	struct RecursiveGenerator
	{
		friend struct promise_type;

		struct promise_type
		{
			using coro_handle = coroutine_handle<promise_type>;
			T value;
			promise_type* Parent = nullptr;
			union
			{
				promise_type* Root;
				promise_type* CurrentStaying;
			};

			promise_type() : Root(this) { }

			T& Value()
			{
				return value;
			}

			void Resume()
			{
				coro_handle::from_promise(*CurrentStaying).resume();
			}

			suspend_always initial_suspend()
			{
				return {};
			}

			void unhandled_exception()
			{
				std::terminate();
			}

			// 这里的参数如果是右值，有的程序 promise 里存的是 value_ptr，那这样安全吗，右值在外面会被析构吗？
			auto yield_value(T t)
			{
				Root->value = move(t);
				return suspend_always();
			}

			RecursiveGenerator get_return_object()
			{
				return coro_handle::from_promise(*this);
			}

			void return_void() {}

			struct FinalAwaiter
			{
				bool await_ready() noexcept
				{
					return false;
				}

				coroutine_handle<> await_suspend(coro_handle handle) noexcept
				{
					auto& current = handle.promise();
					auto parent = current.Parent;

					if (parent != nullptr)
					{
						current.Root->CurrentStaying = parent;
						return coro_handle::from_promise(*parent);
					}

					return noop_coroutine();
				}

				void await_resume() noexcept {}
			};

			FinalAwaiter final_suspend() noexcept
			{
				return {};
			}

			struct YieldSequenceAwaiter
			{
				RecursiveGenerator Generator;

				explicit YieldSequenceAwaiter(RecursiveGenerator generator)
					: Generator(move(generator))
				{ }

				bool await_ready() const noexcept
				{
					return not Generator.handle;
				}

				coro_handle await_suspend(coro_handle handle) const noexcept
				{
					auto& current = handle.promise();
					auto& inner = Generator.handle.promise();
					auto root = current.Root;

					inner.Root = root;
					inner.Parent = &current;
					root->CurrentStaying = &inner;

					return Generator.handle;
				}

				void await_resume() const noexcept
				{
				}
			};

			auto yield_value(RecursiveGenerator generator)
			{
				return YieldSequenceAwaiter(move(generator));
			}
		};
		using coro_handle = coroutine_handle<promise_type>;

		bool MoveNext()
		{
			if (not handle.done())
			{
				// 通过驱动 promise 来实现更丰富的功能
				handle.promise().Resume();
				if (not handle.done())// 说明不是 final_suspend
				{
					return true;
				}
			}

			return false;
		}

		T& Current()
		{
			return handle.promise().Value();
		}

		RecursiveGenerator(coro_handle handle) : handle(handle) {}

		RecursiveGenerator(RecursiveGenerator&& that) noexcept : handle(exchange(that.handle, {}))
		{
		}

		RecursiveGenerator& operator=(RecursiveGenerator const& that) = delete;
		RecursiveGenerator& operator=(RecursiveGenerator&& that) noexcept = delete;

		~RecursiveGenerator()
		{
			if (handle)
			{
				handle.destroy();
			}
		}

	private:
		coro_handle handle;
	};
}
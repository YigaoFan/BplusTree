#pragma once
#include <experimental/coroutine>

namespace Collections
{
	using ::std::move;
	using ::std::experimental::coroutine_handle;
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

			void return_void() { } // 为什么 return_void 可以，这个函数是干嘛用的
		};
		using coro_handle = coroutine_handle<promise_type>;

		bool MoveNext()
		{
			if (not handle.done())
			{
				handle.resume();
				return true;
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
			// 这里树的结构要再想一想
			T value;
			promise_type* Root;
			promise_type* CurrentStaying;
			promise_type* Parent;

			promise_type() : Root(nullptr), CurrentStaying(nullptr), Parent(nullptr)
			{ }

			bool Done() const { return coro_handle::from_promise(*this).done(); }

			T& Value()
			{
				return value;
			}

			void Resume()
			{
				// 下面这里可能有问题，因为下面运行完了，怎么回到上一级
				coro_handle::from_promise(*CurrentStaying).resume();
			}

			suspend_always initial_suspend()
			{
				return {};
			}

			// 想一下 final_suspend 的时候，下面这三个函数是如何调用的
			struct FinalAwaiter
			{
				bool await_ready() noexcept
				{
					return false;
				}

				std::coroutine_handle<> await_suspend(coro_handle handle) noexcept
				{
					auto& current = handle.promise();
					auto parent = current.Parent;
					auto root = current.Root;

					if (parent != nullptr)
					{
						root->CurrentStaying = parent;
						return coro_handle::from_promise(*parent);
					}

					return std::noop_coroutine();
				}
				
				void await_resume() noexcept {}
			};

			FinalAwaiter final_suspend()
			{
				return {};
			}

			void unhandled_exception()
			{
				std::terminate();
			}

			auto yield_value(T t)
			{
				Root->value = move(t);
				return suspend_always();
			}

			// 特化一个 Generator 的版本，或者想一下整个两个类型如何统一
			// auto yield_value(Generator generator)
			struct YieldSequenceAwaiter
			{
				RecursiveGenerator Generator;

				explicit YieldSequenceAwaiter(RecursiveGenerator generator)
					: Generator(move(generator))
				{ }

				bool await_ready() const noexcept
				{
					// 下面这是什么意思？
					return not Generator.handle;
				}

				coro_handle await_suspend(coro_handle handle) const noexcept
				{
					auto& current = handle.promise();
					auto& inner = Generator.handle.promise();
					auto& root = current.Root;
					
					inner.Root = root;
					inner.Parent = &current;
					Root->CurrentStaying = &inner;

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

			auto yield_value(Generator generator)
			{
				// 这个暂时做不到，因为要做到普通的 Generator 里面注册上层的信息，表示当层结束后怎么返回上层
				return YieldSequenceAwaiter(move(generator));
			}

			Generator get_return_object()
			{
				return coro_handle::from_promise(*this);
			}

			void return_void() { }
		};
		using coro_handle = coroutine_handle<promise_type>;

		bool MoveNext()
		{
			if (not handle.done())
			{
				// 通过驱动 promise 来实现更丰富的功能
				handle.promise().Resume();
				return true;
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
			handle.destroy();
		}

	private:
		coro_handle handle;
	};
}
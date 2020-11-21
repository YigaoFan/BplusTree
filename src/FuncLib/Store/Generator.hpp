#include <experimental/coroutine>

namespace FuncLib::Store
{
	using std::experimental::coroutine_handle;
	using std::experimental::suspend_always;

	template <typename T>
	struct Generator
	{
		struct promise_type
		{
			using coro_handle = coroutine_handle<promise_type>;
			T value;
			T &Value()
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
				value = t;
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
}
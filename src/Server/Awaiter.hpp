#pragma once
#include <mutex>
#include <memory>
#include <exception>
#include <experimental/coroutine>

namespace Server
{
	using ::std::exception_ptr;
	using ::std::lock_guard;
	using ::std::mutex;
	using ::std::shared_ptr;

	template <typename Request>
	struct Awaiter
	{
		shared_ptr<Request> RequestPtr;
		exception_ptr ExceptionPtr = nullptr;

		bool await_ready() const noexcept
		{
			return false;
		}

		void await_suspend(auto handle) noexcept
		{
			{
				lock_guard<mutex> lock(RequestPtr->Mutex);

				RequestPtr->Continuation = [=]() mutable
				{
					// printf("request continuation resume\n");
					handle.resume();
				};

				RequestPtr->RegisterException = [this](exception_ptr exceptionPtr) mutable
				{
					ExceptionPtr = exceptionPtr;
				};
			}

			RequestPtr->CondVar.notify_one();
		}

		auto await_resume() const noexcept
		{
			if (ExceptionPtr != nullptr)
			{
				std::rethrow_exception(ExceptionPtr);
			}

			if constexpr (requires(Request* q) { q->Result; })
			{
				return RequestPtr->Result;
			}
		}
	};
}
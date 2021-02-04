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
                    handle.resume();
                    handle.destroy();
                    printf("Awaiter handle destory");
                };

                RequestPtr->RegisterException = [this](exception_ptr exceptionPtr) mutable
                {
                    ExceptionPtr = exceptionPtr;
                    // 这里调用的时候，异常已被 catch，这个异常算 current_exception 吗？ TODO
                    // 看一下协程里的下面这个函数是在什么情况下调用的，看会不会影响 current_exception 的效果
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
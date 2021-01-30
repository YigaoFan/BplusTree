#pragma once
#include <mutex>
#include <memory>
#include <experimental/coroutine>

namespace Server
{
    using ::std::lock_guard;
    using ::std::mutex;
    using ::std::shared_ptr;

    template <typename Request>
    struct Awaiter
    {
        shared_ptr<Request> RequestPtr;

        bool await_ready() const noexcept
        {
            return false;
        }

        void await_suspend(auto handle) const noexcept
        {
            {
                lock_guard<mutex> lock(RequestPtr->Mutex);

                RequestPtr->Success = [=]() mutable
                {
                    handle.resume();
                    handle.destroy();
                };

                RequestPtr->Fail = [promise = &handle.promise()]
                {
                    // 这里调用的时候，异常已被 catch，这个异常算 current_exception 吗？ TODO
                    // 看一下协程里的下面这个函数是在什么情况下调用的，看会不会影响 current_exception 的效果
                    promise->unhandled_exception();
                };
            }

            RequestPtr->CondVar.notify_one();
        }

        auto await_resume() const noexcept
        {
            if constexpr (requires(Request* q) { q->Result; })
            {
                return RequestPtr->Result;
            }
        }
    };
}
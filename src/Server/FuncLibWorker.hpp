#pragma once
#include <queue>
#include <string>
#include "Request.hpp"
#include "ThreadPool.hpp"
#include "../FuncLib/FunctionLibrary.hpp"

namespace Server
{
	using FuncLib::FuncDefTokenReader;
	using FuncLib::FunctionLibrary;
	using FuncLib::FuncType;
	using Json::JsonObject;
	using ::std::move;
	using ::std::queue;
	using ::std::string;

	template <typename T>
	class RequestQueue
	{
	private:
		queue<T> _queue;
		mutex _mutex;

	public:
		RequestQueue() { }

		RequestQueue(RequestQueue&& that) noexcept
			: _queue(MoveFrom(move(that))), _mutex()
		{ }

		T* Add(T t)
		{
			lock_guard<mutex> guard(_mutex);
			while (_queue.front().Done)
			{
				_queue.pop();
			}

			_queue.emplace(move(t));
			return &_queue.back();
		}

	private:
		static queue<T> MoveFrom(RequestQueue&& that)
		{
			lock_guard<mutex> guard(that._mutex);
			return move(that._queue);
		}
	};

	class FuncLibWorker
	{
	private:
		mutex _funcLibMutex;
		FunctionLibrary _funcLib;
		ThreadPool* _threadPool;
		RequestQueue<InvokeRequest> _invokeRequestQueue;
		RequestQueue<AddFuncRequest> _addFuncRequestQueue;
		RequestQueue<RemoveFuncRequest> _removeFuncRequestQueue;
		RequestQueue<SearchFuncRequest> _searchFuncRequestQueue;

	private:
		auto GenerateTask(auto requestPtr, auto specificTask)
		{
			return [this, request = requestPtr, task = move(specificTask)]
			{
				unique_lock<mutex> lock(request->Mutex);
				{
					lock_guard<mutex> libGuard(this->_funcLibMutex);
					task(request);
				}

				request->CondVar.wait(lock, [=]
				{
					return (bool)request->Continuation;
				});
				request->Continuation();
				request->Done = true;
			};
		}

	public:
		FuncLibWorker(FunctionLibrary funcLib) : _funcLib(move(funcLib)) { }

		// 移动构造的时候还没有开始并发访问，所以可以不用加互斥量
		FuncLibWorker(FuncLibWorker&& that) noexcept
			: _funcLibMutex(), _funcLib(move(that._funcLib)),
			  _threadPool(that._threadPool), _invokeRequestQueue(move(that._invokeRequestQueue))
		{ }

		void SetThreadPool(ThreadPool* threadPool)
		{
			_threadPool = threadPool;
		}

		template <typename Request>
		struct Awaiter
		{
			Request* RequestPtr;

			bool await_ready() const noexcept
			{
				return false;
			}

			template <typename Handle>
			void await_suspend(Handle handle) const noexcept
			{
				{
					lock_guard<mutex> lock(RequestPtr->Mutex);

					RequestPtr->Continuation = [=]() mutable
					{
						handle.resume();
						// outside co_routine should without final suspend, so the
						// resource can be free
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

		Awaiter<InvokeRequest> Invoke(FuncType func, JsonObject arg)
		{
			auto requestPtr = _invokeRequestQueue.Add({ {}, move(func), move(arg) });
			_threadPool->Execute(GenerateTask(requestPtr, [this](auto request)
			{
				request->Result = _funcLib.Invoke(request->Func, request->Arg);
			}));

			return { requestPtr };
		}

		Awaiter<AddFuncRequest> AddFunc(vector<string> package, FuncDefTokenReader defReader, string summary)
		{
			auto requestPtr = _addFuncRequestQueue.Add({ {}, move(package), move(defReader), move(summary) });
			_threadPool->Execute(GenerateTask(requestPtr, [this](auto request)
			{
				_funcLib.Add(move(request->Package), move(request->DefReader), move(request->Summary));
			}));

			return { requestPtr };
		}

		Awaiter<RemoveFuncRequest> RemoveFunc(FuncType func)
		{
			auto requestPtr = _removeFuncRequestQueue.Add({ {}, move(func) });
			_threadPool->Execute(GenerateTask(requestPtr, [this](auto request)
			{
				_funcLib.Remove(request->Func);
			}));

			return { requestPtr };
		}

		Awaiter<SearchFuncRequest> SearchFunc(string keyword)
		{
			// {} 构造时，处于后面的有默认构造函数的可以省略，如下面的 Result
			auto requestPtr = _searchFuncRequestQueue.Add({ {}, move(keyword) });
			_threadPool->Execute(GenerateTask(requestPtr, [this](auto request)
			{
				auto g = _funcLib.Search(request->Keyword);
				while (g.MoveNext())
				{
					request->Result.push_back(move(g.Current()));
				}
			}));

			return { requestPtr };
		}
	};
}

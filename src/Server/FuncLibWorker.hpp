#pragma once
#include <mutex>
#include <string>
#include <functional>
#include "ThreadPool.hpp"
#include "../FuncLib/FunctionLibrary.hpp"

namespace Server
{
	using FuncLib::FuncDefTokenReader;
	using FuncLib::FunctionLibrary;
	using FuncLib::FuncType;
	using Json::JsonObject;
	using ::std::function;
	using ::std::lock_guard;
	using ::std::move;
	using ::std::mutex;
	using ::std::scoped_lock;
	using ::std::string;

	struct Request
	{
		Request() = default;

		Request(Request&& that) noexcept : Mutex(), CondVar(), Continuation()
		{
			lock_guard<mutex> guard(that.Mutex);
			Continuation = move(that.Continuation);
		}

		Request& operator= (Request const& that)
		{
			scoped_lock guard(this->Mutex, that.Mutex);
			Continuation = move(that.Continuation);

			return *this;
		}

		mutable mutex Mutex;
		mutable condition_variable CondVar;
		function<void()> Continuation;
	};

	struct InvokeRequest : public Request
	{
		FuncType Func;
		JsonObject Arg;
		JsonObject Result;
	};

	struct AddFuncRequest : public Request
	{
		vector<string> Package;
		FuncDefTokenReader DefReader;
		string Summary;
	};

	template <typename T>
	class RequestQueue
	{
	private:
		vector<T> _queue;
		mutex _mutex;

	public:
		RequestQueue() { }

		RequestQueue(RequestQueue&& that) noexcept
			: _queue(MoveFrom(move(that))), _mutex()
		{ }

		T* Add(T t)
		{
			lock_guard<mutex> guard(_mutex);
			_queue.push_back(move(t));
			return &_queue.back();
		}

		void Remove(T* requestPtr)
		{
			lock_guard<mutex> guard(_mutex);

			for (auto it = _queue.begin(); it != _queue.end(); ++it)
			{
				if (&(*it) == requestPtr)
				{
					_queue.erase(it);
					break;
				}
			}
		}

	private:
		static vector<T> MoveFrom(RequestQueue&& that)
		{
			lock_guard<mutex> guard(that._mutex);
			return move(that._queue);
		}
	};

	// 这里不止是 invoke，还有其他工作
	class FuncLibWorker
	{
	private:
		mutex _funcLibMutex;
		FunctionLibrary _funcLib;
		ThreadPool* _threadPool;
		RequestQueue<InvokeRequest> _invokeRequestQueue;
		RequestQueue<AddFuncRequest> _addFuncRequestQueue;

	public:
		FuncLibWorker(FunctionLibrary funcLib) : _funcLib(move(funcLib)) { }

		// 移动构造的时候还没有开始并发访问，所以可以不用加互斥量
		FuncLibWorker(FuncLibWorker&& that) noexcept
			: _funcLibMutex(), _funcLib(move(that._funcLib)),
			  _threadPool(that._threadPool), _invokeRequestQueue(move(that._invokeRequestQueue))
		{
		}

		void SetThreadPool(ThreadPool* threadPool)
		{
			_threadPool = threadPool;
		}

		template <typename Request>
		struct Task
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

		Task<InvokeRequest> Invoke(FuncType func, JsonObject arg)
		{
			auto requestPtr = _invokeRequestQueue.Add({ {}, move(func), move(arg), {} });
			_threadPool->Execute([this, request = requestPtr]
			{
				unique_lock lock(this->_funcLibMutex);

				request->Result = this->_funcLib.Invoke(request->Func, request->Arg);
				if (not request->Continuation)
				{
					request->CondVar.wait(lock);
				}

				request->Continuation();
				lock.unlock();

				// 下面这个要锁 queue
				this->_invokeRequestQueue.Remove(request);
			});

			return { requestPtr };
		}

		// return is task
		Task<AddFuncRequest> AddFunc(vector<string> package, FuncDefTokenReader defReader, string summary)
		{
			auto requestPtr = _addFuncRequestQueue.Add({ {}, move(package), move(defReader), move(summary) });
			_threadPool->Execute([this, request = requestPtr]
			{
				unique_lock lock(this->_funcLibMutex);

				this->_funcLib.Add(move(request->Package), move(request->DefReader), move(request->Summary));

				if (not request->Continuation)
				{
					request->CondVar.wait(lock);
				}
				
				request->Continuation();
				lock.unlock();
				this->_addFuncRequestQueue.Remove(request);
			});

			return { requestPtr };
		}

		void RemoveFunc(FuncType const& type)
		{

		}

		void SearchFunc(string keyword)
		{
			// return 1;
		}
	};
}

#pragma once
#include <functional>
#include "../FuncLib/FunctionLibrary.hpp"

namespace Server
{
	using FuncLib::FunctionLibrary;
	using FuncLib::FuncType;
	using Json::JsonObject;
	using ::std::function;

	struct InvokeRequest
	{
		FuncType InvokeFunc;
		JsonObject Arg;
		JsonObject Result;
		/// Continuation 使用之前需要判断是否可用
		// 可能没准备好的，要等
		// 用新线程运行 Continuation
		function<void()> Continuation;
	};

	class InvokeRequestQueue
	{
	private:
		vector<InvokeRequest> _requestQueue;

	public:
		InvokeRequest* Add(FuncType funcType, JsonObject arg)
		{
			// 这里应该要加锁
			_requestQueue.push_back({ funcType, arg, {}, {} });
			return &_requestQueue.back();
		}
	};

	class InvokeWorker
	{
	private:
		// FunctionLibrary _funcLib;
		InvokeRequestQueue _requestQueue;

	public:
		InvokeWorker(/* args */)
		{

		}

		struct Task
		{
			InvokeRequest* RequestPtr;

			bool await_ready() const noexcept
			{
				return false;
			}

			template <typename Handle>
			void await_suspend(Handle handle) const noexcept
			{
				RequestPtr->Continuation = [=] () mutable
				{
					handle.resume();
					// outside co_routine should without final suspend, so the 
					// resource can be free
				};
			}

			JsonObject await_resume() const noexcept
			{
				return RequestPtr->Result;
			}
		};
		// 如何像 yield_value 那样，把 InvokeRequest* 包装成一个 awaiter 好让外面把 Continuation 注册进来
		Task Invoke(FuncType type, JsonObject arg)
		{
			return { _requestQueue.Add(move(type), move(arg)) };
		}


		void StartRunBackground()
		{
			// use thread pool to run function invoke
		}
		// how to terminate run? 有这个需求吗？
	};
}

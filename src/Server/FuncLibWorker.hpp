#pragma once
#include <memory>
#include "Request.hpp"
#include "ThreadPool.hpp"
#include "RequestQueue.hpp"
#include "../FuncLib/FunctionLibrary.hpp"

namespace Server
{
	using FuncLib::FunctionLibrary;
	using ::std::make_unique;
	using ::std::move;

	class FuncLibWorker
	{
	private:
		mutex _funcLibMutex;
		FunctionLibrary _funcLib;
		ThreadPool* _threadPool;
		RequestQueue<InvokeFuncRequest> _invokeRequestQueue;
		RequestQueue<AddFuncRequest> _addFuncRequestQueue;
		RequestQueue<RemoveFuncRequest> _removeFuncRequestQueue;
		RequestQueue<SearchFuncRequest> _searchFuncRequestQueue;
		RequestQueue<ModifyFuncPackageRequest> _modifyFuncPackageRequestQueue;

	private:
		auto GenerateTask(auto requestPtr, auto specificTask)
		{
			/// 对 specificTask 提供一个安全访问 request 和 funcLib 的环境
			return [this, request = requestPtr, task = move(specificTask)]
			{
				unique_lock<mutex> lock(request->Mutex);
				struct Guard
				{
					decltype(request) Request;
					decltype(lock)* Lock;

					~Guard()
					{
						if (std::current_exception() != nullptr)
						{
							Request->CondVar.wait(*Lock, [=]
							{
								return (bool)Request->Fail;
							});
							Request->Fail();
						}
						
						// 这里对 fail 的情况要区分下应该 TODO 和 log 里的信息能不能统一起来
						Request->Done = true;
					}
				};

				{
					Guard g{ request, &lock }; // 由于 C++ 逆序析构的原则，Guard 在 lock_guard 之先比较好
					lock_guard<mutex> libGuard(this->_funcLibMutex);
					task(request);
				}

				request->CondVar.wait(lock, [=]
				{
					return (bool)request->Success;
				});
				request->Success();
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

					RequestPtr->Success = [=]() mutable
					{
						handle.resume();
						// outside co_routine should without final suspend, so the
						// resource can be free？
						// handle.destroy(); 看文章说要不要，以及异常情况下这个 handle 怎么释放
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

		Awaiter<InvokeFuncRequest> InvokeFunc(InvokeFuncRequest::Content paras)
		{
			auto requestPtr = _invokeRequestQueue.Add({ {}, move(paras) });
			_threadPool->Execute(GenerateTask(requestPtr, [this](auto request)
			{
				request->Result = _funcLib.Invoke(request->Paras.Func, request->Paras.Arg);
			}));

			return { requestPtr };
		}

		Awaiter<AddFuncRequest> AddFunc(AddFuncRequest::Content paras)
		{
			auto requestPtr = _addFuncRequestQueue.Add({ {}, move(paras) });
			_threadPool->Execute(GenerateTask(requestPtr, [this](auto request)
			{
				using ::std::stringstream;

				stringstream strStream(move(request->Paras.FuncsDef));
				_funcLib.Add(move(request->Paras.Package), { make_unique<stringstream>(move(strStream)) }, move(request->Paras.Summary));
			}));

			return { requestPtr };
		}

		Awaiter<RemoveFuncRequest> RemoveFunc(RemoveFuncRequest::Content paras)
		{
			auto requestPtr = _removeFuncRequestQueue.Add({ {}, move(paras) });
			_threadPool->Execute(GenerateTask(requestPtr, [this](auto request)
			{
				_funcLib.Remove(request->Paras.Func);
			}));

			return { requestPtr };
		}

		Awaiter<SearchFuncRequest> SearchFunc(SearchFuncRequest::Content paras)
		{
			// {} 构造时，处于后面的有默认构造函数的可以省略，曾经如下面的 Result
			auto requestPtr = _searchFuncRequestQueue.Add({ {}, move(paras) });
			_threadPool->Execute(GenerateTask(requestPtr, [this](auto request)
			{
				auto g = _funcLib.Search(request->Paras.Keyword);
				while (g.MoveNext())
				{
					request->Result.push_back(move(g.Current()));
				}
			}));

			return { requestPtr };
		}

		Awaiter<ModifyFuncPackageRequest> ModifyFuncPackage(ModifyFuncPackageRequest::Content paras)
		{
			// {} 构造时，处于后面的有默认构造函数的可以省略，曾经如下面的 Result
			auto requestPtr = _modifyFuncPackageRequestQueue.Add({ {}, move(paras) });
			_threadPool->Execute(GenerateTask(requestPtr, [this](auto request)
			{
				auto& paras = request->Paras;
				_funcLib.ModifyPackageOf(paras.Func, paras.Package);
			}));

			return { requestPtr };
		}
	};
}

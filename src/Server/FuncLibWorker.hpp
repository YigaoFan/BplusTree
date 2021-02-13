#pragma once
#include <memory>
#include <sstream>
#include "../Network/Request.hpp"
#include "ThreadPool.hpp"
#include "Awaiter.hpp"
#include "../FuncLib/FunctionLibrary.hpp"

namespace Server
{
	using FuncLib::FunctionLibrary;
	using Network::AddAdminAccountRequest;
	using Network::AddClientAccountRequest;
	using Network::AddFuncRequest;
	using Network::ContainsFuncRequest;
	using Network::GetFuncsInfoRequest;
	using Network::InvokeFuncRequest;
	using Network::ModifyFuncPackageRequest;
	using Network::RemoveAdminAccountRequest;
	using Network::RemoveClientAccountRequest;
	using Network::RemoveFuncRequest;
	using Network::SearchFuncRequest;
	using ::std::make_shared;
	using ::std::make_unique;
	using ::std::move;

	class FuncLibWorker
	{
	private:
		mutex _funcLibMutex;
		FunctionLibrary _funcLib;
		ThreadPool* _threadPool;

	private:
		/// if ManualManageLock, the lock is locked already
		template <bool ManualManageLock = false>
		auto GenerateTask(auto requestPtr, auto specificTask)
		{
			/// 对 specificTask 提供一个安全访问 request 和 funcLib 的环境
			return [this, request = move(requestPtr), task = move(specificTask)]
			{
				unique_lock<mutex> lock(request->Mutex);
				struct Guard
				{
					decltype(request) Request;

					~Guard()
					{
						Request->Done = true;
					}
				};

				auto handleException = [&]
				{
					request->CondVar.wait(lock, [&]
					{
						return (bool)request->RegisterException;
					});
					request->RegisterException(std::current_exception());
				};
				Guard g{ request }; // 由于 C++ 逆序析构的原则，Guard 在 lock_guard 之先比较好
				if constexpr (ManualManageLock)
				{
					unique_lock<mutex> libLock(this->_funcLibMutex);
					try
					{
						task(request, &libLock);
					}
					catch (...)
					{
						handleException();
					}
				}
				else
				{
					lock_guard<mutex> libGuard(this->_funcLibMutex);
					try
					{
						task(request);
					}
					catch (...)
					{
						handleException();
					}
				}

				request->CondVar.wait(lock, [&]
				{
					return (bool)request->Continuation;
				});
				request->Continuation();
			};
		}

	public:
		FuncLibWorker(FunctionLibrary funcLib) : _funcLib(move(funcLib)) { }

		// 移动构造的时候还没有开始并发访问，所以可以不用加互斥量
		FuncLibWorker(FuncLibWorker&& that) noexcept
			: _funcLibMutex(), _funcLib(move(that._funcLib)), _threadPool(that._threadPool)
		{ }

		void SetThreadPool(ThreadPool* threadPool)
		{
			_threadPool = threadPool;
		}

		Awaiter<InvokeFuncRequest> InvokeFunc(InvokeFuncRequest::Content paras)
		{
			auto requestPtr = make_shared<InvokeFuncRequest>(InvokeFuncRequest{ {}, move(paras) });
			_threadPool->Execute(GenerateTask<true>(requestPtr, [this](auto request, unique_lock<mutex>* lockPtr)
			{
				auto invoker = _funcLib.GetInvoker(request->Paras.Func, request->Paras.Arg);
				lockPtr->unlock();
				request->Result = invoker();
			}));

			return { requestPtr };
		}

		Awaiter<AddFuncRequest> AddFunc(AddFuncRequest::Content paras)
		{
			auto requestPtr = make_shared<AddFuncRequest>(AddFuncRequest{ {}, move(paras) });
			_threadPool->Execute(GenerateTask(requestPtr, [this](auto request)
			{
				using ::std::istringstream;

				istringstream strStream(move(request->Paras.FuncsDef));
				_funcLib.Add(move(request->Paras.Package), { make_unique<istringstream>(move(strStream)) }, move(request->Paras.Summary));
			}));

			return { requestPtr };
		}

		Awaiter<RemoveFuncRequest> RemoveFunc(RemoveFuncRequest::Content paras)
		{
			auto requestPtr = make_shared<RemoveFuncRequest>(RemoveFuncRequest{ {}, move(paras) });
			_threadPool->Execute(GenerateTask(requestPtr, [this](auto request)
			{
				_funcLib.Remove(request->Paras.Func);
			}));

			return { requestPtr };
		}

		Awaiter<SearchFuncRequest> SearchFunc(SearchFuncRequest::Content paras)
		{
			// {} 构造时，处于后面的有默认构造函数的可以省略，曾经如下面的 Result
			auto requestPtr = make_shared<SearchFuncRequest>(SearchFuncRequest{ {}, move(paras) });
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
			auto requestPtr = make_shared<ModifyFuncPackageRequest>(ModifyFuncPackageRequest{ {}, move(paras) });
			_threadPool->Execute(GenerateTask(requestPtr, [this](auto request)
			{
				auto& paras = request->Paras;
				_funcLib.ModifyPackageOf(paras.Func, paras.NewPackage);
			}));

			return { requestPtr };
		}

		Awaiter<ContainsFuncRequest> ContainsFunc(ContainsFuncRequest::Content paras)
		{
			auto requestPtr = make_shared<ContainsFuncRequest>(ContainsFuncRequest{ {}, move(paras) });
			_threadPool->Execute(GenerateTask(requestPtr, [this](auto request)
			{
				auto& paras = request->Paras;
				request->Result = _funcLib.Contains(paras.Func);
			}));

			return { requestPtr };
		}

		Awaiter<GetFuncsInfoRequest> GetFuncsInfo()
		{
			auto requestPtr = make_shared<GetFuncsInfoRequest>(GetFuncsInfoRequest{ {} });
			_threadPool->Execute(GenerateTask(requestPtr, [this](auto request)
			{
				auto g = _funcLib.FuncTypes();
				while (g.MoveNext())
				{
					request->Result.push_back(move(g.Current()));
				}
			}));

			return { requestPtr };
		}
	};
}

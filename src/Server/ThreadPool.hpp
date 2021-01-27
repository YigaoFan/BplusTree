#pragma once
#include <queue>
#include <mutex>
#include <memory>
#include <thread>
#include <functional>
#include <condition_variable>

namespace Server
{
	using ::std::condition_variable;
	using ::std::function;
	using ::std::lock_guard;
	using ::std::make_shared;
	using ::std::move;
	using ::std::mutex;
	using ::std::queue;
	using ::std::shared_ptr;
	using ::std::thread;
	using ::std::unique_lock;

	// Code from: https://www.zhihu.com/question/27908489/answer/355105668
	class ThreadPool
	{
	private:
		struct Data
		{
			mutex Mutex;
			condition_variable CondVar;
			bool IsShutdown = false;
			queue<function<void()>> Tasks;
		};

		shared_ptr<Data> _data;
	public:
		ThreadPool(size_t threadCount) : _data(make_shared<Data>())
		{
			for (size_t i = 0; i < threadCount; ++i)
			{
				thread([data = _data]
				{
					unique_lock<mutex> lock(data->Mutex);
					for (;;)
					{
						if (not data->Tasks.empty())
						{
							auto current = move(data->Tasks.front());
							data->Tasks.pop();
							lock.unlock();
							try
							{
								current();
							}
							catch(...)
							{ }

							lock.lock();
						}
						else if (data->IsShutdown)
						{
							break;
						}
						else
						{
							data->CondVar.wait(lock);
						}
					}
				}).detach();
			}
		}

		ThreadPool(ThreadPool&& that) noexcept = default;

		template <typename Task>
		void Execute(Task&& task)
		{
			{
				lock_guard<mutex> lock(_data->Mutex);
				_data->Tasks.emplace(std::forward<Task>(task));
			}

			_data->CondVar.notify_one();
		}
		
		~ThreadPool()
		{
			if (_data != nullptr)
			{
				{
					lock_guard<mutex> lock(_data->Mutex);
					_data->IsShutdown = true;
				}

				_data->CondVar.notify_all();
			}
		}
	};
}
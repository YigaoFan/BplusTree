#include <thread>

namespace Server
{
	using ::std::thread;

	class ThreadPool
	{
	private:
		/* data */
	public:
		ThreadPool(/* args */);

		template <typename Task>
		void Execute(Task&& task)
		{

		}
		
		~ThreadPool();
	};
	
	ThreadPool::ThreadPool(/* args */)
	{
	}
	
	ThreadPool::~ThreadPool()
	{
	}
	
}
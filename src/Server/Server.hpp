#include <string_view>
#include "../Json/Parser.hpp"
#include "ThreadPool.hpp"
#include "RequestListener.hpp"
#include "RequestResponder.hpp"
#include "InvokeWorker.hpp"
#include "../Logger/Logger.hpp"

namespace Server
{
	using ::std::string_view;

	// Server 里有个中间层对象隔离来控制 FunctionLibrary，加锁以及任务队列可以在这里面加
	// 对于每个合法的调用请求生成唯一的 id 标识唯一性
	class Server
	{
	private:
		decltype(Log::MakeLoggerWith(STRING("%h %l %u %t \"%r\" %>s %b \"%{Referer}i\" \"%{User-agent}i\""))) _logger;
		ThreadPool _threadPool;
		RequestListener _listener; // 这个可能要调整下
		RequestResponder _responder; // 这个可能要调整下
		InvokeWorker _invokeWorker;
	public:
		Server();
		void Init();
		void Run();


		// admin 路线
		void HandleAdminConnect()
		{

		}

		// client 路线
		void HandleClientConnect()
		{
			_threadPool.Execute(AllWorkflow);
		}

		void AllWorkflow()
		{
			// 前期通讯和鉴权
			// 调用
			HandleInvoke();
		}

		struct Awaiter
		{

		};

		Awaiter HandleInvoke(string_view funcInvokeRequest)
		{
			// 下面的代码需要异常处理
			auto invokeInfo = Json::Parse(funcInvokeRequest);
			auto funcName = invokeInfo["name"].GetString();// 这里 funcName 要注意和函数库的 type 包含同样的信息
			auto arg = invokeInfo["arg"];
			FuncType type;// TODO
			// if step above has error, then set Continuation type to respond error client
			// log between below steps
			co_await _invokeWorker.Invoke(type, move(arg), /* continuation */);
			
			// await call worker to invoke, then outside put
			// send result to client
		}

		~Server();
	};
	
	Server::Server()
	{
	}
	
	Server::~Server()
	{
	}
		
}

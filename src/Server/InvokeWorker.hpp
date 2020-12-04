#include "../FuncLib/FunctionLibrary.hpp"

namespace Server
{
	using FuncLib::FunctionLibrary;
	using FuncLib::FuncType;
	using Json::JsonObject;

	class InvokeWorker
	{
	private:
		FunctionLibrary _funcLib;

	public:
		InvokeWorker(/* args */);
		template <typename Continuation>
		void Invoke(FuncType const& type, JsonObject arg, Continuation continuation)
		{
			// 这里应该要加锁
		}

		~InvokeWorker();
	};
	
	InvokeWorker::InvokeWorker(/* args */)
	{
	}
	
	InvokeWorker::~InvokeWorker()
	{
	}
		
}

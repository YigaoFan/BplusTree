#pragma once
#include "../Json/Parser.hpp"
#include "../Json/JsonConverter/JsonConverter.hpp"
#include "../FuncLib/Compile/FuncType.hpp"
#include "ServiceBase.hpp"
#include "promise_Void.hpp"

namespace Server
{
	using FuncLib::Compile::FuncType;
	using ::std::move;

	// 先写出来，然后再考虑可测试的写法，是想把网络给 mock 起来，这样不用网络也能测试
	class ClientService : private ServiceBase
	{
	private:
		using Base = ServiceBase;
	public:
		using Base::Base;

		void Run()
		{
#define ASYNC_HANDLER_OF_WITH_RESULT(NAME)                                 \
	[this]() -> Void                                                       \
	{                                                                      \
		auto paras = ReceiveFromClient<CAT(NAME, Request)::Content>();     \
		auto result = co_await _funcLibWorker->NAME(move(paras));          \
		auto response = Json::JsonConverter::Serialize(result).ToString(); \
		_responder->RespondTo(_peer, response);                            \
	}
			// LoopAcquire<InvokeFuncRequest::Content>(ASYNC_HANDLER_OF_WITH_RESULT(InvokeFunc));
			while (true) ASYNC_HANDLER_OF_WITH_RESULT(InvokeFunc)();
			// how client send eof or disconnect? 让它自然析构就行或者先 shutdown 然后 close
			// 对于每个合法的调用请求生成唯一的 id 标识唯一性
			
			// component like pipe，然后不符合成功了进行下一步，错误了抛异常
			// Parse >> InvokeFunc >> Respond
			// 上面这些的错误如何处理，是抛出来统一做异常处理，不选择：不允许抛异常，都自己在里面处理
#undef ASYNC_HANDLER_OF_WITH_RESULT
		}

		// Void InvokeFuncOf(InvokeFuncRequest::Content paras)
		// {
		// 	// para 是形参，arg 是实参
			
		// 	// log between below steps
		// 	// 下面代码需要异常处理
		// 	auto result = co_await _funcLibWorker->InvokeFunc(move(paras));
		// 	auto respond = result.ToString(); // TODO add other content
		// 	_responder->RespondTo(_peer, respond);
		// }
	};	
}

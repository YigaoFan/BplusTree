#pragma once
#include "../Json/Parser.hpp"
#include "../Json/JsonConverter/JsonConverter.hpp"
#include "../FuncLib/Compile/FuncType.hpp"
#include "../Log/FormatMap.hpp"
#include "ServiceBase.hpp"
#include "promise_Void.hpp"

namespace Server
{
	using FuncLib::Compile::FuncType;
	using Log::ResultStatus;
	using ::std::move;

	// 先写出来，然后再考虑可测试的写法，是想把网络给 mock 起来，这样不用网络也能测试
	class ClientService : private ServiceBase
	{
	private:
		using Base = ServiceBase;
	public:
		using Base::Base;

		template <typename UserLogger>
		void Run(UserLogger userLogger)
		{
			Loop(ASYNC_HANDLER(InvokeFunc));
			// how client send eof or disconnect? 让它自然析构就行或者先 shutdown 然后 close
			// 对于每个合法的调用请求生成唯一的 id 标识唯一性
			
			// component like pipe，然后不符合成功了进行下一步，错误了抛异常
			// 上面这些的错误如何处理，是抛出来统一做异常处理，不选择：不允许抛异常，都自己在里面处理
		}
		
		// para 是形参，arg 是实参
	};	
}

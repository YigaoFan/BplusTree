#pragma once
#include "ServiceBase.hpp"

namespace Server
{
	class ClientService : private ServiceBase
	{
	private:
		using Base = ServiceBase;
	public:
		using Base::Base;

		void Run(auto userLogger)
		{
			AsyncLoop(move(userLogger), ASYNC_FUNC_LIB_HANDLER(InvokeFunc));
			// how client send eof or disconnect? 让它自然析构就行或者先 shutdown 然后 close
			// component like pipe，然后不符合成功了进行下一步，错误了抛异常
			// 上面这些的错误如何处理，是抛出来统一做异常处理，不选择：不允许抛异常，都自己在里面处理
		}
	};	
}

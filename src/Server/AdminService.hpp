#pragma once
#include "ServiceBase.hpp"

namespace Server
{
	enum ServiceOption
	{
		AddFunc,
		RemoveFunc,
		SearchFunc,
		ModifyFuncPackage,
		ContainsFunc,
	};
}

namespace Json::JsonConverter
{
	using Server::ServiceOption;

	template <>
	JsonObject Serialize(ServiceOption const& option)
	{
		return Serialize(static_cast<int>(option));
	}

	template <>
	ServiceOption Deserialize(JsonObject const& jsonObj)
	{
		return static_cast<ServiceOption>(Deserialize<int>(jsonObj));
	}
}

namespace Server
{
	class AdminService : private ServiceBase
	{
	private:
		using Base = ServiceBase;

	public:
		using Base::Base;

		void Run(auto userLogger)
		{
			// 那出异常怎么设置失败呢
			// 这里 LoopAcquireThenDispatch 用 userLogger 来记录这里面的日志
			// 各个 handler 通过 capture 自行处理会产生的各自的 requestLogger
			// 这个产生了不必要的 requestLogger，这点之后应该要处理
			AsyncLoopAcquireThenDispatch<ServiceOption>(
				move(userLogger),
				_peer,
				[](auto serviceOption)
				{
					return static_cast<int>(serviceOption);
				},
				// 下面的顺序需要和 ServiceOption enum 的顺序一致
				ASYNC_HANDLER(AddFunc),
				ASYNC_HANDLER(RemoveFunc),
				ASYNC_HANDLER(SearchFunc),
				ASYNC_HANDLER(ModifyFuncPackage),
				ASYNC_HANDLER(ContainsFunc)
			);
		}
	};
}

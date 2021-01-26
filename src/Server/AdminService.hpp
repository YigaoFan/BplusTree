#pragma once
#include <type_traits>
#include "../Log/FormatMap.hpp"
#include "ServiceBase.hpp"
#include "promise_Void.hpp"
#include "RequestId.hpp"

namespace Server
{
	enum ServiceOption
	{
		AddFunc,
		RemoveFunc,
		SearchFunc,
		ModifyFuncPackage,
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
	using Log::ResultStatus;
	using ::std::is_same_v;
	using ::std::move;
	using namespace Json;

	class AdminService : private ServiceBase
	{
	private:
		using Base = ServiceBase;

	public:
		using Base::Base;

		template <typename UserLogger>
		void Run(UserLogger userLogger)
		{
// 以及生成 response 那一步可以拿个函数包一下，这样可以进行一些调整操作

	// 那出异常怎么设置失败呢

			// 这里 LoopAcquireThenDispatch 用 userLogger 来记录这里面的日志
			// 各个 handler 通过 capture 自行处理会产生的各自的 requestLogger
			// 这个产生了不必要的 requestLogger，这点之后应该要处理
			LoopAcquireThenDispatch<ServiceOption>(
				move(userLogger),
				[this](auto serviceOption)
				{
					return static_cast<int>(serviceOption);
				},
				// 下面的顺序需要和 ServiceOption enum 的顺序一致
				ASYNC_HANDLER(AddFunc),
				ASYNC_HANDLER(RemoveFunc),
				ASYNC_HANDLER(SearchFunc),
				ASYNC_HANDLER(ModifyFuncPackage)
				// 这里的 lambda 里面还可以用 LoopAcquireThenDispatch，这样就可以写复杂情况了
			);
		}
// #undef CAT 因为 FlyTest 也用这个，所以不能 undef，之后把部分代码移入 cpp 文件应该可以 undef 了
// #undef PRIMITIVE_CAT
	};
}

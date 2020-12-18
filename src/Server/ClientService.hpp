#pragma once
#include "../Json/Parser.hpp"
#include "../Json/JsonConverter/JsonConverter.hpp"
#include "../FuncLib/Compile/FuncType.hpp"
#include "ServiceBase.hpp"
#include "promise_Void.hpp"

namespace Server
{
	struct InvokeInfo
	{
		FuncType Func;
		JsonObject Arg;
	};
}

namespace Json::JsonConverter
{
	using FuncLib::Compile::FuncType;

	template <>
	JsonObject Serialize(FuncType const &type)
	{
#define nameof(VAR) #VAR
		auto [returnType, funcName, argTypes, package] = type;
		JsonObject::_Object obj;
		obj.insert({ nameof(returnType), Serialize(returnType) });
		obj.insert({ nameof(funcName),   Serialize(funcName) });
		obj.insert({ nameof(argTypes),   Serialize(argTypes) });
		obj.insert({ nameof(package),    Serialize(package) });

		return JsonObject(move(obj));
	}

	template <>
	FuncType Deserialize(JsonObject const& jsonObj)
	{
		auto returnType = Deserialize<string>(jsonObj[nameof(returnType)]);
		auto funcName = Deserialize<string>(jsonObj[nameof(funcName)]);
		auto argTypes = Deserialize<vector<string>>(jsonObj[nameof(argTypes)]);
		auto package = Deserialize<vector<string>>(jsonObj[nameof(package)]);

		return FuncType(returnType, funcName, argTypes, package);
	}

	using Server::InvokeInfo;

	template <>
	JsonObject Serialize(InvokeInfo const& invokeInfo)
	{
		auto [func, arg] = invokeInfo;
		JsonObject::_Object obj;
		obj.insert({ nameof(func), Serialize(func) });
		obj.insert({ nameof(arg),  arg });

		return JsonObject(move(obj));
	}

	template <>
	InvokeInfo Deserialize(JsonObject const& jsonObj)
	{
		auto func = Deserialize<FuncType>(jsonObj[nameof(func)]);
		auto arg = jsonObj[nameof(arg)];

		return InvokeInfo{ func, arg };
	}
#undef nameof
}

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
			LoopAcquire<InvokeInfo>([this](auto invokeInfo)
			{
				InvokeFuncOf(move(invokeInfo));
			});
			// how client send eof or disconnect? 让它自然析构就行或者先 shutdown 然后 close
			// 对于每个合法的调用请求生成唯一的 id 标识唯一性
			
			// component like pipe，然后不符合成功了进行下一步，错误了抛异常
			// Parse >> Invoke >> Respond
			// 上面这些的错误如何处理，是抛出来统一做异常处理，不选择：不允许抛异常，都自己在里面处理
		}

		Void InvokeFuncOf(InvokeInfo invokeInfo)
		{
			// para 是形参，arg 是实参
			
			// log between below steps
			// 下面代码需要异常处理
			auto result = co_await _funcLibWorker->Invoke(invokeInfo.Func, invokeInfo.Arg);
			auto respond = result.ToString(); // TODO add other content
			_responder->RespondTo(_peer, respond);
		}
	};	
}

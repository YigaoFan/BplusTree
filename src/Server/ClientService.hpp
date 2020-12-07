#pragma once
#include <utility>
#include <array>
#include <string_view>
#include <experimental/coroutine>
#include "../Json/Parser.hpp"
#include "../Json/JsonConverter/JsonConverter.hpp"
#include "../FuncLib/Compile/FuncType.hpp"
#include "Responder.hpp"
#include "InvokeWorker.hpp"
#include "BasicType.hpp"

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
		obj.insert({ nameof(returnType), make_shared<JsonObject>(Serialize(returnType)) });
		obj.insert({ nameof(funcName),   make_shared<JsonObject>(Serialize(funcName)) });
		obj.insert({ nameof(argTypes),   make_shared<JsonObject>(Serialize(argTypes)) });
		obj.insert({ nameof(package),    make_shared<JsonObject>(Serialize(package)) });

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
#define nameof(VAR) #VAR
		auto [func, arg] = invokeInfo;
		JsonObject::_Object obj;
		obj.insert({ nameof(func), make_shared<JsonObject>(Serialize(func)) });
		obj.insert({ nameof(arg),   make_shared<JsonObject>(arg) });

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
	// using namespace Json;
	using FuncLib::Compile::FuncType;
	using ::std::array;
	using ::std::move;
	using ::std::string_view;
	using ::std::experimental::suspend_never;

	// template <typename T>
	// T Deserialize(string_view s)
	// {
	// 	// 有两处用到了类似的功能
	// 	auto jsonObj = Json::Parse(s);
	// 	return JsonConverter::Deserialize<T>(jsonObj["funcType"]);
	// }

	// 先写出来，然后再考虑可测试的写法，是想把网络给 mock 起来，这样不用网络也能测试
	class ClientService
	{
	private:
		Socket _peer;
		InvokeWorker* _invokeWorker;
		Responder* _responder;
	public:
		ClientService(Socket peer, InvokeWorker* invokeWorker, Responder* responder)
			: _peer(move(peer)), _invokeWorker(invokeWorker), _responder(responder)
		{ }

		void Run()
		{
			array<char, 1024> buff;

			while (true)
			{
				// if step above has error, respond error to client
				auto n = _peer.receive(asio::buffer(buff));
				auto input = string_view(buff.data(), n);
				auto infoJsonObj = Json::Parse(input);
				auto invokeInfo = Json::JsonConverter::Deserialize<InvokeInfo>(infoJsonObj);
				// 对于每个合法的调用请求生成唯一的 id 标识唯一性
				InvokeFuncOf(move(invokeInfo));
			}
			
			// component like pipe，然后不符合成功了进行下一步，错误了抛异常
			// Parse >> Invoke >> Respond
			// 上面这些的错误如何处理，是抛出来统一做异常处理，不选择：不允许抛异常，都自己在里面处理

			// 最终结束，Socket 中的连接要怎么断开？让上层处理，每一次来一个函数请求，调用一个这个函数
		}

		struct Void
		{
			struct promise_type
			{
				suspend_never initial_suspend()
				{
					return {};
				}

				// 不 final_suspend 了，还需要显式 destroy handle 吗？
				suspend_never final_suspend()
				{
					return {};
				}

				void unhandled_exception()
				{
					// std::terminate(); 这里可能要处理下
					// 要把异常给出去
				}

				Void get_return_object()
				{
					return {};
				}

				void return_void() { }
			};
		};

		Void InvokeFuncOf(InvokeInfo invokeInfo)
		{
			// para 是形参，arg 是实参
			
			// log between below steps
			// 下面代码需要异常处理
			auto result = co_await _invokeWorker->Invoke(invokeInfo.Func, invokeInfo.Arg);
			auto respond = result.ToString(); // TODO add other content
			// send result to client
			_responder->RespondTo(_peer, respond);
		}
	};	
}

#pragma once
#include <tuple>
#include <vector>
#include <string>
#include <utility>
#include <type_traits>
#include "../Basic/TypeTrait.hpp"
#include "../Basic/Exception.hpp"
#include "Predefine.hpp"
#include "TypeToString.hpp"
#include "../Json/Parser.hpp"
#include "../Network/Request.hpp"
#include "../Network/Socket.hpp"
#include "../Network/IoContext.hpp"
#include "../Network/Util.hpp"
#include "../TestFrame/Util.hpp"

using namespace Test;

namespace RemoteInvokeLib
{
	using Basic::FuncTraits;
	using Basic::InvalidOperationException;
	using FuncLib::Compile::FuncType;
	using Json::JsonObject;
	using ::Network::HandleOperationResponse;
	using Network::InvokeFuncRequest;
	using Network::IoContext;
	using Network::LoginRequest;
	using Network::Socket;
	using ::std::forward;
	using ::std::forward_as_tuple;
	using ::std::index_sequence;
	using ::std::invoke_result_t;
	using ::std::is_same_v;
	using ::std::make_index_sequence;
	using ::std::move;
	using ::std::string;
	using ::std::vector;

	class RemoteInvoker
	{
	private:
		IoContext _ioContext;
		string _serverIp;
		int _port;
		LoginRequest::Content _loginInfo;

		RemoteInvoker(string serverIp, int port, LoginRequest::Content loginInfo)
			: _ioContext(), _serverIp(move(serverIp)), _port(port), _loginInfo(move(loginInfo))
		{ }

	public:
		static RemoteInvoker New(string serverIp, int port, string username, string password)
		{
			return RemoteInvoker(move(serverIp), port, { move(username), move(password) });
		}

		template <typename Func, typename... Args>
		auto Invoke(vector<string> package, string const& funcName, Args&&... args) -> invoke_result_t<Func, Args...>
		{
			using F = FuncTraits<Func>;
			auto getArgsType = []<auto... Idxs>(index_sequence<Idxs...>)
			{
				return vector<string>
				{
					TypeToString<typename F::template Arg<Idxs>::Type>::Result()...,
				};
			};
			auto getArgsJsonObj = [&]<auto... Idxs>(index_sequence<Idxs...>)
			{
				auto argsTuple = forward_as_tuple(args...);
				JsonObject::_Array a
				{
					Json::JsonConverter::Serialize<typename F::template Arg<Idxs>::Type>(std::get<Idxs>(argsTuple))...,
				};
				return JsonObject(move(a));
			};
			constexpr auto argCount = F::ArgCount;
			static_assert(argCount == sizeof...(Args), "passed args' count not correct");

			using ReturnType = invoke_result_t<Func, Args...>;
			string returnType = TypeToString<ReturnType>::Result();
			vector<string> argsType = getArgsType(make_index_sequence<argCount>());
			auto argsJsonObj = getArgsJsonObj(make_index_sequence<argCount>());

			FuncType type(move(returnType), funcName, move(argsType), move(package));
			auto request = Json::JsonConverter::Serialize(InvokeFuncRequest::Content{ move(type), move(argsJsonObj) });

			return DoInvokeOnRemote<ReturnType>(request);
		}

		template <typename Func, typename... Args>
		auto Invoke(PredefinePackage package, string const& funcName, Args&&... args)
		{
			return Invoke(ToVec(package), funcName, forward<Args>(args)...);
		}

	private:
		template <typename ReturnType>
		ReturnType DoInvokeOnRemote(JsonObject const& request)
		{
			// 这里的对方等待这里的回应会占用不必要的时间
			auto peer = _ioContext.GetConnectedSocketTo(_serverIp, _port);
			
			peer.Send("hello server");
			auto r1 = peer.Receive();
			if (r1 != "server ok")
			{
				throw InvalidOperationException("server response greet error: " + r1);
			}
			log("greet ok %s\n", r1.c_str());

			peer.Send(Json::JsonConverter::Serialize(_loginInfo).ToString());
			auto r2 = peer.Receive();
			if (r2 != "server ok. hello client.")
			{
				throw InvalidOperationException("server response login error: " + r2);
			}
			log("login ok %s\n", r2.c_str());

			peer.Send(request.ToString());
			auto id = peer.Receive();
			log("receive id %s\n", id.c_str());
			auto response = peer.Receive();
			log("response %s\n", response.c_str());
			return HandleOperationResponse<ReturnType>(response);
		}
	};
}

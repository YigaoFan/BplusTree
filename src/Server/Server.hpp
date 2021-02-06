#pragma once
#include <atomic>
#include <vector>
#include <utility>
#include <sstream>
#include <filesystem>
#include "../Log/Logger.hpp"
#include "ThreadPool.hpp"
#include "../Network/NetworkAcceptor.hpp"
#include "../Network/IoContext.hpp"
#include "BusinessAcceptor.hpp"
#include "Responder.hpp"
#include "../FuncLib/FunctionLibrary.hpp"
#include "FuncLibWorker.hpp"
#include "AccountManager.hpp"

namespace Server
{
	namespace fs = ::std::filesystem;
	using FuncLib::FuncsDefReader;
	using FuncLib::FunctionLibrary;
	using Network::IoContext;
	using Network::NetworkAcceptor;
	using Network::Socket;
	using ::std::atomic;
	using ::std::forward;
	using ::std::istringstream;
	using ::std::move;
	using ::std::pair;
	using ::std::vector;

	class Server
	{
	private:
		atomic<bool> _shutdown = false;
		ThreadPool _threadPool;
		NetworkAcceptor _netAcceptor;
		BusinessAcceptor _businessAcceptor;
		FuncLibWorker _funcLibWorker;
		AccountManager _accountManager;

	public:
		Server(ThreadPool threadPool, NetworkAcceptor netAcceptor, BusinessAcceptor businessAcceptor, FuncLibWorker funcLibWorker, AccountManager accountManager)
			: _threadPool(move(threadPool)), _netAcceptor(move(netAcceptor)),
			  _businessAcceptor(move(businessAcceptor)),
			  _funcLibWorker(move(funcLibWorker)),
			  _accountManager(move(accountManager))
		{
			_businessAcceptor.SetThreadPool(&_threadPool);
			_funcLibWorker.SetThreadPool(&_threadPool);
			_accountManager.SetThreadPool(&_threadPool);

			auto shutdown = [this]()
			{
				this->_shutdown = true;
				this->_netAcceptor.Close();
			};
			_businessAcceptor.SetServiceFactory(
				[&](shared_ptr<Socket> socket, Responder *responder) mutable { return ClientService(move(socket), &_funcLibWorker, responder); },
				[&, shutdown=shutdown](shared_ptr<Socket> socket, Responder *responder) mutable { return AdminService(move(socket), &_funcLibWorker, responder, &_accountManager, shutdown); });
			_businessAcceptor.SetAccountManager(&_accountManager);

			SetAcceptCallback();
		}

		void StartRunBackground()
		{
			_businessAcceptor.StartAcceptBackground();
		}
		
		static auto New(IoContext& ioContext, int port)
		{
			fs::path serverDir = R"(./server)";
			auto firstSetup = false;
			if ((not fs::exists(serverDir)) or (not fs::is_directory(serverDir)))
			{
				firstSetup = true;
				fs::create_directory(serverDir);
			}

			auto funcLib = FunctionLibrary::GetFrom(serverDir);
			auto accountData = AccountData::GetFrom(serverDir);
			if (firstSetup)
			{
				InitBasicFuncTo(funcLib);
				accountData.Add(false, "yigao fan", "hello world");
				accountData.Add(true, "admin", "hello world");
			}

			auto n = thread::hardware_concurrency();
			ThreadPool threadPool(n);
			Responder responder;
			auto logger = Log::MakeLogger(serverDir / "server.log");
			auto acceptor = BusinessAcceptor(move(responder), move(logger));
			auto funcLibWorker = FuncLibWorker(move(funcLib));
			auto accountManager = AccountManager(move(accountData));
			NetworkAcceptor netAcceptor = ioContext.GetNetworkAcceptorOf(port);
			// 下面这一步里构造里牵涉到了指针，然后你这个对象是要 move 出去的
			return Server(move(threadPool), move(netAcceptor), move(acceptor), move(funcLibWorker), move(accountManager));
		}

	private:
		void SetAcceptCallback()
		{
			_netAcceptor.AsyncAccept([this]<typename... Ts>(Ts&&... ts)
			{
				_businessAcceptor.HandleAccept(forward<Ts>(ts)...);

				if (not _shutdown)
				{
					SetAcceptCallback();
				}
			});
		}

		static void InitBasicFuncTo(FunctionLibrary& lib)
		{
			auto def =
				"int Zero()\n"
				"{\n"
				"	return 0;\n"
				"}\n"
				"int One()\n"
				"{\n"
				"	return 1;\n"
				"}\n"
				"#include <string>\n"
				"using std::string;\n"
				"string Add(string s1, string s2)\n"
				"{\n"
				"	return s1 + s2;\n"
				"}\n"
				"string Hello(string s1)\n"
				"{\n"
				"	return \"Hello \" + s1;\n"
				"}\n";

			auto [pack, defReader] = MakeFuncDefReader(def);
			lib.Add(pack, move(defReader), "");
		}

		static pair<vector<string>, FuncsDefReader> MakeFuncDefReader(string funcDef)
		{
			vector<string> package
			{
				"Basic",
			};
			istringstream is(funcDef);
			auto reader = FuncsDefReader(make_unique<istringstream>(move(is)));
			return pair(move(package), move(reader));
		}
	};
}

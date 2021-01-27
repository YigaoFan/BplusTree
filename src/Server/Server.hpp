#pragma once
#include <vector>
#include <utility>
#include <sstream>
#include <filesystem>
#include "../Log/Logger.hpp"
#include "ThreadPool.hpp"
#include "NetworkAcceptor.hpp"
#include "IoContext.hpp"
#include "BusinessAcceptor.hpp"
#include "Responder.hpp"
#include "../FuncLib/FunctionLibrary.hpp"
#include "FuncLibWorker.hpp"

namespace Server
{
	namespace fs = ::std::filesystem;
	using FuncLib::FuncsDefReader;
	using FuncLib::FunctionLibrary;
	using ::std::istringstream;
	using ::std::move;
	using ::std::pair;
	using ::std::vector;

	class Server
	{
	private:
		ThreadPool _threadPool;
		NetworkAcceptor _netAcceptor;
		BusinessAcceptor _businessAcceptor;
		FuncLibWorker _funcLibWorker;

	public:
		Server(ThreadPool threadPool, NetworkAcceptor netAcceptor, BusinessAcceptor businessAcceptor, FuncLibWorker funcLibWorker)
			: _threadPool(move(threadPool)), _netAcceptor(move(netAcceptor)),
			  _businessAcceptor(move(businessAcceptor)),
			  _funcLibWorker(move(funcLibWorker))
		{
			_businessAcceptor.SetThreadPool(&_threadPool);
			_funcLibWorker.SetThreadPool(&_threadPool);

			_businessAcceptor.SetServiceFactory(
				[&](shared_ptr<Socket> socket, Responder *responder) mutable { return ClientService(move(socket), &_funcLibWorker, responder); },
				[&](shared_ptr<Socket> socket, Responder *responder) mutable { return AdminService(move(socket), &_funcLibWorker, responder); });

			_netAcceptor.AsyncAccept(
				std::bind(&BusinessAcceptor::HandleAccept, &_businessAcceptor,
						  std::placeholders::_1, std::placeholders::_2));
		}

		void StartRunBackground()
		{
			_businessAcceptor.StartAcceptBackground();
		}
		
		static auto New(IoContext& ioContext, int port)
		{
			fs::path serverDir = R"(./server)";
			auto funcLib = FunctionLibrary::GetFrom(serverDir);
			if ((not fs::exists(serverDir)) or (not fs::is_directory(serverDir)))
			{
				fs::create_directory(serverDir);
				InitBasicFuncTo(funcLib);
			}

			auto n = thread::hardware_concurrency();
			ThreadPool threadPool(n);
			Responder responder;
			auto logger = Log::MakeLogger(serverDir / "server.log");
			auto acceptor = BusinessAcceptor(move(responder), move(logger));
			auto funcLibWorker = FuncLibWorker(move(funcLib));
			NetworkAcceptor netAcceptor = ioContext.GetNetworkAcceptorOf(port);
			return Server(move(threadPool), move(netAcceptor), move(acceptor), move(funcLibWorker));
		}

	private:
		static void InitBasicFuncTo(FunctionLibrary& lib)
		{
			auto def =
				"int Zero()\n"
				"{\n"
				"	return 0;\n"
				"}\n"
				"int Add(int a, int b)\n"
				"{\n"
				"	return a + b;\n"
				"}\n"
				"#include <string>\n"
				"using std::string;\n"
				"string Add(string s1, string s2)\n"
				"{\n"
				"	return s1 + s2;\n"
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

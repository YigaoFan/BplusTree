#include "../TestFrame/FlyTest.hpp"
#include <fstream>
#define MOCK_NET
#include "../Server.hpp"
#include "../Network/Request.hpp"

namespace Svr = Server;
using namespace Svr;
using namespace Json;
using FuncLib::Compile::FuncType;
using ::std::ofstream;

TESTCASE("Server Test")
{
	Svr::IoContext io;
	auto s = Svr::Server::New(io, 13);
	s.StartRunBackground();

	SECTION("Client Service", false)
	{
		vector<string> messages
		{
			"hello server"
		};
		LoginRequest::Content info
		{
			"yigao fan",
			"hello world",
		};
		messages.push_back(JsonConverter::Serialize(info).ToString());
		auto f = FuncType("int", "Zero", {}, { "Basic" });
		InvokeFuncRequest::Content r
		{
			f,
			JsonObject(),
		};
		messages.push_back(JsonConverter::Serialize(r).ToString());

		io.SetSendMessages(messages);
		auto n = 1400;
		for (auto i = 0; i < n; ++i)
		{
			io.Run();
		}

		// 733 per min 8 thread
		// 866 per min 8 thread (after optimize invoke)
		// 792 per min 16 thread
		// 823 per min 12 thread
		std::this_thread::sleep_for(std::chrono::seconds(180));
	}

	SECTION("Admin Service")
	{
		using namespace Network;

		vector<string> messages
		{
			"hello server"
		};
		LoginRequest::Content info
		{
			"admin",
			"hello world",
		};
		messages.push_back(JsonConverter::Serialize(info).ToString());
		messages.push_back(JsonConverter::Serialize(AdminServiceOption::RemoveFunc).ToString());
		auto f = FuncType("int", "Zero", {}, { "Basic" });
		InvokeFuncRequest::Content r
		{
			f,
			JsonObject(),
		};
		messages.push_back(JsonConverter::Serialize(r).ToString());

		io.SetSendMessages(messages);
		auto n = 1;
		for (auto i = 0; i < n; ++i)
		{
			io.Run();
		}

		std::this_thread::sleep_for(std::chrono::seconds(25));
	}
}

// 下面这个改为大写驼峰，其他的也改一下 TODO
DEF_TEST_FUNC(TestMockServer)
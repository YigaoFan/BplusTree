#include "../TestFrame/FlyTest.hpp"
#define MOCK_NET
#include "../Server.hpp"

namespace Svr = Server;
using namespace Svr;
using namespace Json;

TESTCASE("Server Test")
{
    Svr::IoContext io;
    auto s = Svr::Server::New(io, 13);
    s.StartRunBackground();

    vector<string> messages
    {
        "hello server"
    };
    LoginInfo info
    {
        "yigao fan",
        "hello world",
    };
    messages.push_back(JsonConverter::Serialize(info).ToString());
    auto f = FuncType("int", "Zero", {}, {"Basic"});
    InvokeFuncRequest::Content r
    {
        f,
        JsonObject(),
    };
    messages.push_back(JsonConverter::Serialize(r).ToString());

    io.SetSendMessages(messages);
    auto n = 10000;
    for (auto i = 0; i < n; ++i)
    {
        io.Run();
    }
    std::this_thread::sleep_for(std::chrono::minutes(6));
}

// 下面这个改为大写驼峰，其他的也改一下 TODO
DEF_TEST_FUNC(TestServer)
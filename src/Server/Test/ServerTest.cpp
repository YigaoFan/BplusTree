#include "../TestFrame/FlyTest.hpp"
#define MOCK_NET
#include "../Server.hpp"

namespace Svr = Server;

TESTCASE("Server Test")
{
    Svr::IoContext io;
    auto s = Svr::Server::New(io, 13);
    s.StartRunBackground();
    io.SetSendMessages({ "Hello Server" });
    io.Run();
}

// 下面这个改为大写驼峰，其他的也改一下 TODO
DEF_TEST_FUNC(TestServer)
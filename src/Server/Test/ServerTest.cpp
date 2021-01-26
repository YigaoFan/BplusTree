#include "../TestFrame/FlyTest.hpp"
// #define MOCK_NET
#include "../Server.hpp"

using namespace Server;

TESTCASE("Server Test")
{
    IoContext io;
    auto s = ::Server::Server::New(io, 13);
    s.StartRunBackground();
    io.Run();
}

// 下面这个改为大写驼峰，其他的也改一下 TODO
DEF_TEST_FUNC(TestServer)
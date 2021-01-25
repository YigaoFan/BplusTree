#include "../../TestFrame/FlyTest.hpp"
#include "../Server.hpp"

TESTCASE("Server Test")
{
    asio::io_context io;
    auto s = Server::New(io, 13);
    s.StartRunBackground();
    io.run();
}

// 下面这个改为大写驼峰，其他的也改一下 TODO
DEF_TEST_FUNC(TestServer)
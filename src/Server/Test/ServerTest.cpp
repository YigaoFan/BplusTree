#include "../TestFrame/FlyTest.hpp"
#include "../Server.hpp"
#include "../TestFrame/Util.hpp"

namespace Svr = Server;
using namespace Svr;
using namespace Test;

TESTCASE("Server Test")
{
	Svr::IoContext io;
	auto s = Svr::Server::New(io, 8888);
	s.StartRunBackground();
	io.Run();
}

DEF_TEST_FUNC(TestServer)
#define MEMO_CHECK false
#if MEMO_CHECK
#include <crtdbg.h>
#endif

#define COLLECTIONS_TEST_SWITCH true
#define JSON_TEST_SWITCH true
#define FUNC_LIB_TEST_SWITCH true
#define SERVER_TEST_SWITCH false

#if COLLECTIONS_TEST_SWITCH
#include "Btree/Test/TestSuite.hpp"
#define COLLECTIONS_TEST(FUNC) FUNC
#else
#define COLLECTIONS_TEST(FUNC)
#endif

#if JSON_TEST_SWITCH
#include "Json/Test/TestSuite.hpp"
#define JSON_TEST(FUNC) FUNC
#else
#define JSON_TEST(FUNC)
#endif

#if FUNC_LIB_TEST_SWITCH
#include "FuncLib/Test/TestSuite.hpp"
#define FUNC_LIB_TEST(FUNC) FUNC
#else
#define FUNC_LIB_TEST(FUNC)
#endif

#if SERVER_TEST_SWITCH
#include "Server/Test/TestSuite.hpp"
#define SERVER_TEST(FUNC) FUNC
#else
#define SERVER_TEST(FUNC)
#endif

int main()
{
	// resumable res = foo();
	// while (res.resume());// 这里应该就是调度器的作用
	// return 0;
#if MEMO_CHECK
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
#endif
	COLLECTIONS_TEST(Collections::Test::AllTest(true));
	JSON_TEST(Json::Test::AllTest(true));
	FUNC_LIB_TEST(FuncLib::Test::AllTest(false));
	SERVER_TEST(Server::Test::AllTest(false));

#if MEMO_CHECK
	 _CrtDumpMemoryLeaks();
#endif
	return 0;
}



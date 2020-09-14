#include <string>
#include "../../TestFrame/FlyTest.hpp"
#include "../ByteConverter.hpp"

using namespace FuncLib;
using ::std::string;

TESTCASE("Byte converter test")
{
    // The file should be a interface, that just provide bytes and other functions
    SECTION("Basic type")
    {
        auto i = 10;
        auto a = ByteConverter<int>::WriteDown(i);
    }
    
    SECTION("POD type")
    {
        struct Sample
        {
            int a;
            int b;
        };

        Sample s{1, 2};
        auto data = ByteConverter<Sample>::WriteDown(s);
    }

    SECTION("String")
    {
        string s = "Hello World";
        auto chars = ByteConverter<string>::WriteDown(s);
    }
}

DEF_TEST_FUNC(byteConverterTest)
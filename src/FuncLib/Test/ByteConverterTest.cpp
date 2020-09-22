#include <string>
#include <filesystem>
#include "../../TestFrame/FlyTest.hpp"
#include "../ByteConverter.hpp"
#include "../Store/FileWriter.hpp"

using namespace FuncLib;
using namespace FuncLib::Store;
using ::std::string;
using ::std::filesystem::path;

TESTCASE("Byte converter test")
{
    auto filename = make_shared<path>("ByteConverterTest");
    auto writer = make_shared<FileWriter>(filename);

    SECTION("Basic type")
    {
        auto i = 10;
        ByteConverter<int>::WriteDown(i, writer);
        static_assert(ByteConverter<int>::SizeStable);

        auto f = 1.23F;
        ByteConverter<float>::WriteDown(f, writer);
        static_assert(ByteConverter<float>::SizeStable);

        auto d = 1.23;
        ByteConverter<double>::WriteDown(d, writer);
        static_assert(ByteConverter<double>::SizeStable);

        auto b = false;
        ByteConverter<bool>::WriteDown(b, writer);
        static_assert(ByteConverter<bool>::SizeStable);
    }
    
    SECTION("POD type")
    {
        struct Sample
        {
            int a;
            int b;
        };

        Sample s{1, 2};
        ByteConverter<Sample>::WriteDown(s, writer);
        static_assert(ByteConverter<Sample>::SizeStable);
    }

    SECTION("NON-POD type")
    {
        struct Sample
        {
            int a;
            string b;
        };

        Sample s{ 1, "Hello World" };
        ByteConverter<Sample>::WriteDown(s, writer);
        static_assert(!ByteConverter<Sample>::SizeStable);
    }

    SECTION("String")
    {
        string s = "Hello World";
        ByteConverter<string>::WriteDown(s, writer);
        static_assert(!ByteConverter<string>::SizeStable);
    }

    SECTION("Btree")
    {
        // TODO
    }
}

DEF_TEST_FUNC(byteConverterTest)
#include <memory>
#include <array>
#include <vector>
#include <cstddef>
#include "../../TestFrame/FlyTest.hpp"
#include "../File.hpp"
#include <cstdio>

using ::std::shared_ptr;
using ::std::make_shared;
using ::std::byte;
using namespace FuncLib;

// like allocator
TESTCASE("File test")
{
    auto filename = "dataTest";
    auto f = make_shared<File>(filename);
    byte b { 65 };
    array<byte, 3> data
    {
        b,
        b,
        b,
    };
    auto pos = f->Write<decltype(data)>(data);
    auto a = f->Read<data.size()>(pos);
    ASSERT(a.size() == data.size());
    for (auto i = 0; i < data.size(); ++i)
    {
        ASSERT(a[i] == b);
    }

    SECTION("Append data")
    {
        // test has type data read
        byte b2 { 66 };
        vector<byte> data2(100, b2);
        auto p = f->Write<decltype(data2)>(data2);
        auto v = f->Read(p, data2.size());
        ASSERT(v.size() == data2.size());
        for (auto i = 0; i < data2.size(); ++i)
        {
            ASSERT(v[i] == b2);
        }
    }

    auto v = f->Read(pos, sizeof(data));
    ASSERT(v.size() == data.size());
    for (auto i = 0; i < data.size(); ++i)
    {
        ASSERT(v[i] == b);
    }

    remove(filename);
}

DEF_TEST_FUNC(fileTest)
#include <memory>
#include <limits>
#include <cmath>
#include "../../TestFrame/FlyTest.hpp"
#include "../TypeConverter.hpp"

using namespace FuncLib;
using namespace Collections;
using namespace std;

TESTCASE("Type converter test")
{
    // TODO add TypeConvert::
    auto file = make_shared<File>("data");
    auto lessThan = [](int const &a, int const &b) { return a < b; };
    auto predPtr = make_shared<LessThan<int>>(lessThan);

    SECTION("Basic type Convert")
    {
        auto i = 3;
        auto c_i = TypeConverter<int>::ConvertFrom(i, file);
        ASSERT(i == c_i);


        auto f = 1.23;// double
        auto c_f = TypeConverter<double>::ConvertFrom(f, file);
        ASSERT(f == c_f);

        auto c = 'a';
        auto c_c = TypeConverter<char>::ConvertFrom(c, file);
        ASSERT(c == c_c);

        // TODO string
        // string s = "Hello World";
        // auto c_s = TypeConverter<char>::ConvertFrom(c, file);
        // ASSERT(c == c_c);
    }

    SECTION("POD struct Convert")
    {
        struct A
        {
            int i;
            float f;
        };

        A a{ 1, 1.23 };
        auto c_a = TypeConverter<A>::ConvertFrom(a, file);
        ASSERT(c_a.i == a.i);
        ASSERT(c_a.f == a.f);
    }

    SECTION("NON-POD struct Convert")
    {
        // TODO
        struct A
        {
            int i;
            float f;
            string s;
        };
        A a{ 1, 1.23, "Hello World" };

        // auto c_a = TypeConverter<A>::ConvertFrom(a, file);
        // ASSERT(c_a.i == a.i);
        // ASSERT(c_a.f == a.f);
    }

    SECTION("LiteVector Convert")
    {
        // Not have this function
	}

    SECTION("Elements Convert")
    {
        Elements<int, int, 4> es(predPtr);
        es.Add({ 1, 1 });
        es.Add({ 2, 2 });
        auto c_es = TypeConverter<decltype(es)>::ConvertFrom(es, file);
        ASSERT(c_es.Count() == 2);
        ASSERT(c_es[0].first = 1);
        ASSERT(c_es[0].second = 1);
        ASSERT(c_es[1].second = 2);
        ASSERT(c_es[1].second = 2);

        // Elements<string, int, 4> es(predPtr); TODO
    }

    SECTION("LeafNode Convert")
    {
        using Leaf = LeafNode<int, int, 4>;
        Leaf l(predPtr);
        // auto c_l = TypeConverter<decltype(l)>::ConvertFrom(l, file);
    }

    SECTION("MiddleNode Convert")
    {
    }

    SECTION("NodeBase Convert")
    {
    }

    SECTION("Btree Convert")
    {
    }
}

DEF_TEST_FUNC(typeConverterTest)
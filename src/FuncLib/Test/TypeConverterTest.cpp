#include <memory>
#include "../../TestFrame/FlyTest.hpp"
#include "../TypeConverter.hpp"

using namespace FuncLib;
using ::std::shared_ptr;

TESTCASE("Type converter test")
{
    auto file = make_shared<File>("data");
    auto i = 3;
    auto c = TypeConverter<int>::ConvertFrom(i, file);

    SECTION("Sample")
	{
	
	}
}

DEF_TEST_FUNC(typeConverterTest)
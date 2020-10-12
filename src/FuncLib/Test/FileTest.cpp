#include <memory>
#include <array>
#include <vector>
#include <cstddef>
#include <cstdio>
#include "../../TestFrame/FlyTest.hpp"
#include "../ByteConverter.hpp"

using namespace std;
using namespace FuncLib;
using namespace FuncLib::Store;


TESTCASE("File test")
{
	// 函数库要写一些元信息和校验数据，平台限定
	// 需要使用 std::bit_cast 来转换使用不相关类型吗？
	auto filename = "fileTest";
	auto file = File::GetFile("fileTest"); //如果没有就创建文件

	auto [posLable, obj] = file->New(1);
	auto obj1 = file->Read<int>(posLable);
	ASSERT(obj == obj1);
	ASSERT(*obj == 1);
	file->Store(posLable, obj);
	file->Delete(posLable, obj);

	remove(filename);
}

DEF_TEST_FUNC(fileTest)
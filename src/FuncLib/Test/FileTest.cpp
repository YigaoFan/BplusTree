#include <memory>
#include <array>
#include <vector>
#include <cstddef>
#include <cstdio>
#include "../../TestFrame/FlyTest.hpp"
#include "../Persistence/ByteConverter.hpp"

using namespace std;
using namespace FuncLib;
using namespace FuncLib::Store;


TESTCASE("File test")
{
	// 函数库要写一些元信息和校验数据，平台限定
	// 需要使用 std::bit_cast 来转换使用不相关类型吗？
	auto filename = "fileTest";
	auto file = File::GetFile("fileTest"); //如果没有就创建文件

	auto [posLabel, obj] = file->New(1);
	auto obj1 = file->Read<int>(posLabel);
	ASSERT(obj == obj1);
	ASSERT(*obj == 1);
	file->Store(posLabel, obj);
	file->Delete(posLabel, obj);

	remove(filename);
}

DEF_TEST_FUNC(fileTest)
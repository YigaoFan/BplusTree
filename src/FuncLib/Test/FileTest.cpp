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
	auto filename = "fileTest";
	auto file = File::GetFile("fileTest");//如果没有就创建文件
	auto [obj, pos] = file->New(1);

	// {
	//     byte b{65};
	//     array<byte, 3> data
	//     {
	//         b,
	//         b,
	//         b,
	//     };
	//     // auto pos = f->Write<decltype(data)>(data);
	//     // auto a = f->Read<data.size()>(pos);
	//     ASSERT(a.size() == data.size());
	//     for (auto i = 0; i < data.size(); ++i)
	//     {
	//         ASSERT(a[i] == b);
	//     }

	//     SECTION("Append data")
	//     {
	//         // test has type data read
	//         byte b2{ 66 };
	//         vector<byte> data2(100, b2);
	//         auto p = f->Write<decltype(data2)>(data2);
	//         auto v = f->Read(p, data2.size());
	//         ASSERT(v.size() == data2.size());
	//         for (auto i = 0; i < data2.size(); ++i)
	//         {
	//             ASSERT(v[i] == b2);
	//         }
	//     }

	//     auto v = f->Read(pos, sizeof(data));
	//     ASSERT(v.size() == data.size());
	//     for (auto i = 0; i < data.size(); ++i)
	//     {
	//         ASSERT(v[i] == b);
	//     }
	// } // Because File will Flush in destructor which will create a file,
	//   // add {} to let destruct come first before below remove

	// remove(filename);
}

DEF_TEST_FUNC(fileTest)
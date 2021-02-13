#include <string>
#include <filesystem>
#include "../../TestFrame/FlyTest.hpp"
#include "../Persistence/ByteConverter.hpp"
#include "../Store/ObjectBytes.hpp"
#include "../Store/FileReader.hpp"
#include "Util.hpp"

using namespace FuncLib;
using namespace FuncLib::Store;
using namespace FuncLib::Test;
using namespace FuncLib::Persistence;
using namespace std;

TESTCASE("Byte converter test")
{
	auto filename = "TestByteConverter";
	auto path = MakeFilePath(filename);
	{
		auto writer = ObjectBytes(0);// TODO pos_label
		auto reader = FileReader::MakeReader(nullptr, path, 0);

		SECTION("Basic type")
		{
			auto i = 10;
			ByteConverter<int>::WriteDown(i, &writer);
			static_assert(ByteConverter<int>::SizeStable);
			ASSERT(i == ByteConverter<int>::ReadOut(&reader));

			auto f = 1.23F;
			ByteConverter<float>::WriteDown(f, &writer);
			static_assert(ByteConverter<float>::SizeStable);
			ASSERT(f == ByteConverter<float>::ReadOut(&reader));

			auto d = 1.23;
			ByteConverter<double>::WriteDown(d, &writer);
			static_assert(ByteConverter<double>::SizeStable);
			ASSERT(d == ByteConverter<double>::ReadOut(&reader));

			auto b = false;
			ByteConverter<bool>::WriteDown(b, &writer);
			static_assert(ByteConverter<bool>::SizeStable);
			ASSERT(b == ByteConverter<bool>::ReadOut(&reader));
		}

		SECTION("POD type")
		{
			struct Sample
			{
				int a;
				int b;
				int c;
				int d;
				int e;
			};

			Sample s{1, 2, 3, 4, 5,};
			ByteConverter<Sample>::WriteDown(s, &writer);
			static_assert(ByteConverter<Sample>::SizeStable);
			auto c_s = ByteConverter<Sample>::ReadOut(&reader);
			ASSERT(s.a == c_s.a);
			ASSERT(s.b == c_s.b);
			ASSERT(s.c == c_s.c);
			ASSERT(s.d == c_s.d);
			ASSERT(s.e == c_s.e);
		}

		SECTION("NON-POD type")
		{
			struct Sample
			{
				int a;
				string b;
			};

			Sample s{1, "Hello World"};
			ByteConverter<Sample>::WriteDown(s, &writer);
			static_assert(!ByteConverter<Sample>::SizeStable);
			auto c_s = ByteConverter<Sample>::ReadOut(&reader);
			ASSERT(s.a == c_s.a);
			ASSERT(s.b == c_s.b);
		}

		// 为什么同样是多次写入，其他 SECTION 就没有出问题？只有涉及 string 的有问题
		// 只有写入 char 序列时会改变会将其他部分置零吗？
		SECTION("String")
		{
			string s = "Hello World";
			ByteConverter<string>::WriteDown(s, &writer);
			static_assert(!ByteConverter<string>::SizeStable);
			auto c_s = ByteConverter<string>::ReadOut(&reader);
			ASSERT(s == c_s);
		}

		SECTION("Btree")
		{
			// TODO
		}
	}

	remove(filename);
}

DEF_TEST_FUNC(TestByteConverter)
#include <memory>
#include <array>
#include <vector>
#include <cstddef>
#include <cstdio>
#include "Util.hpp"
#include "../../TestFrame/FlyTest.hpp"
#define private public
#include "../Persistence/ByteConverter.hpp"
#include "../Persistence/TypeConverter.hpp"

using namespace std;
using namespace FuncLib;
using namespace FuncLib::Persistence;
using namespace FuncLib::Store;
using namespace FuncLib::Test;

TESTCASE("File test")
{
	auto filename = "fileTest";
	// // 函数库要写一些元信息和校验数据，平台限定
	// // 需要使用 std::bit_cast 来转换使用不相关类型吗？

	SECTION("Simple", false)
	{
		auto file = File::GetFile(filename);
		auto [posLabel, obj] = file->New(1);
		auto obj1 = file->Read<int>(posLabel);
		ASSERT(obj == obj1);
		ASSERT(*obj == 1);
		file->Store(posLabel, obj);
		// file->Delete(posLabel, obj);
		Cleaner c(filename);
	}
	
	SECTION("Complex(int version)", false)
	{
		Cleaner c(filename);
		auto file = File::GetFile(filename);
		using namespace Collections;
		using Tree = Btree<4, int, int>; // 之后用 string 作为 key，value
		Tree b;
		auto n = 4;
		auto dn = 4;
		for (auto i = 0; i < n; ++i)
		{
			b.Add({ i, i });
		}
		
		auto t = TypeConverter<Tree>::ConvertFrom(b, file.get());
		// printf("after convert\n");
		// operate on converted tree
		for (auto i = 0; i < n; ++i)
		{
			ASSERT(t.ContainsKey(i));
		}

		for (auto i = n; i < n + dn; ++i)
		{
			t.Add({ i, i });
			ASSERT(t.ContainsKey(i));
		}

		for (auto i = 0; i < n + dn; ++i)
		{
			auto newValue = 100;
			t.ModifyValue(i, newValue);
			ASSERT(t.GetValue(i) == newValue);
		}

		for (auto i = 0; i < n + dn; ++i)
		{
			t.Remove(i);
			ASSERT(not t.ContainsKey(i));
		}

		auto [label, treeObj] = file->New(move(t));
		// printf("start store\n");
		file->Store(label, treeObj);
	}

	SECTION("Complex(string version)", false)
	{
		using T = string;
		Cleaner c(filename);

		auto file = File::GetFile(filename);
		using namespace Collections;
		using Tree = Btree<4, T, T>;
		Tree b;
		auto n = 4;
		auto dn = 100;
		// auto dn = 14;
		for (auto i = 0; i < n; ++i)
		{
			b.Add({ to_string(i), to_string(i) });
		}

		auto t = TypeConverter<Tree>::ConvertFrom(b, file.get());
		// printf("after convert\n");
		ASSERT(t.Count() == n);
		// operate on converted tree
		for (auto i = 0; i < n; ++i)
		{
			ASSERT(t.ContainsKey(to_string(i)));
		}

		for (auto i = n; i < n + dn; ++i)
		{
			auto k = to_string(i);
			UniqueDiskRef<T> ks(MakeUnique(k, file.get()));
			UniqueDiskRef<T> vs(MakeUnique(k, file.get()));
			// printf("start add %s\n", k.c_str());
			t.Add({move(ks), move(vs)});
			// printf("after add %s\n", k.c_str());
			// t.CheckTree();
			ASSERT(t.ContainsKey(k));
			ASSERT(t.GetValue(k) == k);
		}

		{
			auto ks = t.Keys();
			ASSERT(ks.size() == n + dn);
		}

		for (auto i = 0; i < n + dn; ++i)
		{
			auto k = to_string(i);
			t.Remove(k);
			ASSERT(not t.ContainsKey(k));
			
			auto ks = t.Keys();
			ASSERT(ks.size() == (n + dn - i - 1));
		}

		// printf("start new\n");
		auto [label, treeObj] = file->New(move(t));
		// printf("start store\n");
		file->Store(label, treeObj);
	}

	SECTION("Store and Read")
	{
		using T = string;
		auto filename = "store_read";
		constexpr pos_label l = 300;

		// Cleaner c(filename);
		using Tree = Btree<4, T, T, StorePlace::Memory>;
		using DiskTree = Btree<4, T, T, StorePlace::Disk>;
		auto const n = 2;
		auto const dn = 30;

		SECTION("Test seek", false)
		{
			using ::std::filesystem::exists;
			auto filename = "seek_test";
			if (not exists(filename))
			{
				ofstream f(filename);
			}

			{
				fstream fs(filename, fstream::binary | fstream::in | fstream::out);
				fs.seekp(1000, fstream::beg);
				char const s[] = "All is well~";
				fs.write(s, sizeof(s));
				fs.flush();
			}

			{
				fstream fs(filename, fstream::binary | fstream::in | fstream::out);
				fs.seekp(3000, fstream::beg);
				char const s[] = "Hello world!";
				fs.write(s, sizeof(s));
				fs.flush();
			}

		}

		SECTION("Store")
		{
			auto file = File::GetFile(filename);
			using namespace Collections;
			Tree b;
			// auto dn = 14;
			for (auto i = 0; i < n; ++i)
			{
				b.Add({to_string(i), to_string(i)});
			}

			auto t = TypeConverter<Tree>::ConvertFrom(b, file.get());
			// printf("after convert\n");
			ASSERT(t.Count() == n);
			// operate on converted tree
			for (auto i = 0; i < n; ++i)
			{
				ASSERT(t.ContainsKey(to_string(i)));
			}

			for (auto i = n; i < n + dn; ++i)
			{
				auto k = to_string(i);
				UniqueDiskRef<T> ks(MakeUnique(k, file.get()));
				UniqueDiskRef<T> vs(MakeUnique(k, file.get()));
				// printf("start add %s\n", k.c_str());
				t.Add({move(ks), move(vs)});
				// printf("after add %s\n", k.c_str());
				// t.CheckTree();
				ASSERT(t.ContainsKey(k));
				ASSERT(t.GetValue(k) == k);
			}

			{
				auto ks = t.Keys();
				ASSERT(ks.size() == n + dn);
			}

			// printf("start new\n");
			auto [label, treeObj] = file->New(l, move(t));
			printf("store label %d\n", label);
			file->Store(label, treeObj);
		}

		SECTION("Read")
		{
			auto file = File::GetFile(filename);
			printf("Start read test\n");
			auto t = file->Read<DiskTree>(l);
			// printf("read num %d\n", t->Count());
			ASSERT(t->Count() == (n + dn));

			for (auto i = 0; i < n + dn; ++i)
			{
				ASSERT(t->ContainsKey(to_string(i)));
				ASSERT(t->GetValue(to_string(i)) == to_string(i));
			}
		}

		SECTION("Add", false)
		{
			auto file = File::GetFile(filename);
			printf("Start add test\n");
			auto t = file->Read<DiskTree>(l);
			ASSERT(t->Count() == (n + dn));

			for (auto i = 0; i < n + dn; ++i)
			{
				ASSERT(t->ContainsKey(to_string(i)));
				ASSERT(t->GetValue(to_string(i)) == to_string(i));
			}
		}
	}
}

DEF_TEST_FUNC(TestFile)
#include <array>
#include <algorithm>
#include <iostream>
#include <random>
#include "../../TestFrame/FlyTest.hpp"
#include "../Btree.hpp"
#include "../CollectionException.hpp"
#include "../../Basic/Exception.hpp"
#include "../Enumerator.hpp"
#include "Util.hpp"
#include "MemoryMemo.hpp"

using ::std::array;
using ::std::move;
using namespace Collections;
using namespace Basic;

TESTCASE("Str-Str btree test")
{
	auto kv0 = make_pair<string, string>("0", "Hello world, fanyigao");
	auto kv1 = make_pair<string, string>("1", "Hello world, fanyigao");
	auto kv2 = make_pair<string, string>("2", "Hello world, fanyigao");
	auto kv3 = make_pair<string, string>("3", "Hello world, fanyigao");
	auto kv4 = make_pair<string, string>("4", "Hello world, fanyigao");
	auto kv5 = make_pair<string, string>("5", "Hello world, fanyigao");
	auto kv6 = make_pair<string, string>("6", "Hello world, fanyigao");
	auto kv7 = make_pair<string, string>("7", "Hello world, fanyigao");
	auto kv8 = make_pair<string, string>("8", "Hello world, fanyigao");
	auto kv9 = make_pair<string, string>("9", "Hello world, fanyigao");
	auto kv10 = make_pair<string, string>("10", "Hello world, fanyigao");
	auto kv11 = make_pair<string, string>("11", "Hello world, fanyigao");
	auto kv12 = make_pair<string, string>("12", "Hello world, fanyigao");
	auto kv13 = make_pair<string, string>("13", "Hello world, fanyigao");
	auto kv14 = make_pair<string, string>("14", "Hello world, fanyigao");
	auto kv15 = make_pair<string, string>("15", "Hello world, fanyigao");
	auto kv16 = make_pair<string, string>("16", "Hello world, fanyigao");
	auto kv17 = make_pair<string, string>("17", "Hello world, fanyigao");
	auto kv18 = make_pair<string, string>("18", "Hello world, fanyigao");
	auto kv19 = make_pair<string, string>("19", "Hello world, fanyigao");

	array<decltype(kv0), 9> basicKeyValueArray 
	{
		kv0, kv1, kv2, kv3, kv4, kv5, kv6, kv7, kv8,
	};

	array<decltype(kv0), 11> secondKeyValueArray
	{
		kv9, kv10, kv11, kv12, kv13, kv14, kv15, kv16, kv17, kv18, kv19,
	};

	array<decltype(kv0), 20> completeKeyValueArray
	{
		kv0, kv1, kv2, kv3, kv4, kv5, kv6, kv7, kv8, kv9,
		kv10, kv11, kv12, kv13, kv14, kv15, kv16, kv17, kv18, kv19,
	};

	auto lessThan = [](string const& a, string const& b) { return a < b; };

	SECTION("Duplicate tree cons")
	{
		using Btr = Collections::Btree<4, string, string>;
		array<pair<string, string>, 10> dupKeyValueArray
		{
			kv0, kv1, kv2, kv3, kv4,
			kv5, kv6, kv7, kv8, kv2,
		};
		ASSERT_THROW(DuplicateKeyException<string>, Btr(lessThan, dupKeyValueArray));
	}

#define ADD_LOT_FROM(ARRAY)  for (auto& p : ARRAY) { btree.Add(p); ASSERT(btree.ContainsKey(p.first)); }
	SECTION("Empty cons")
	{
		using Btr = Collections::Btree<4, string, string>;
		Btr btree(lessThan);
		ASSERT(btree.Empty());

		SECTION("Add a lot")
		{
			ADD_LOT_FROM(completeKeyValueArray);
		}
	}

	SECTION("Count more than BtreeOrder btree")
	{
		using Btr = Collections::Btree<4, string, string>;
		Btr btree(lessThan, basicKeyValueArray);

		SECTION("Move cons")
		{
			auto n = btree.Count();
			auto b = move(btree);
			ASSERT(btree.Count() == 0);
			ASSERT(b.Count() == n);
		}

		SECTION("Copy cons")
		{
			auto b = btree;
			ASSERT(b.Count() == btree.Count());
			b.Remove("3");
			ASSERT(!b.ContainsKey("3"));
			ASSERT(btree.ContainsKey("3"));
			//b.~Btr(); 
			// This manual call destructor, out scope will call, too
			// pointer will not be set to null after call destructor of unique_ptr on Windows 
			// so cannot call destructor multiple times on Windows
			// But Mac platform will set to null
			// Maybe the destructor implement standard of unique_ptr is undefined.
		}

		SECTION("Normal function")
		{
			auto keys = btree.Keys();
			MapContainers(CreateRefEnumerator(basicKeyValueArray), CreateRefEnumerator(keys), [&successCount](auto& e1, auto& e2)
			{
				ASSERT(e1.first == e2);
			});

#define	BASIC_KV_FOR_EACH for (auto const& p : basicKeyValueArray)
			BASIC_KV_FOR_EACH
			{
				ASSERT(btree.ContainsKey(p.first));
				ASSERT(btree.GetValue(p.first) == p.second);
			}

			ASSERT_THROW(KeyNotFoundException, btree.GetValue("10"));
			btree.Add(make_pair("10", "d"));
			ASSERT(btree.Count() == (keys.size() + 1));
			ASSERT(btree.ContainsKey("10"));
			ASSERT(btree.GetValue("10") == "d");
			btree.Remove("10");
			ASSERT(!btree.ContainsKey("10"));
			ASSERT(btree.Count() == keys.size());

			btree.ModifyValue(kv0.first, "d");
			ASSERT(btree.GetValue(kv0.first) == "d");
		}

		SECTION("Add a lot")
		{
			ADD_LOT_FROM(secondKeyValueArray);
		}

		SECTION("Duplicate add")
		{
			BASIC_KV_FOR_EACH
			{
				ASSERT_THROW(DuplicateKeyException<string>, btree.Add(p));
			}
		}

		SECTION("Extreme remove")
		{
			BASIC_KV_FOR_EACH
			{
				btree.Remove(p.first);
				ASSERT(!btree.ContainsKey(p.first));
			}

			ASSERT(btree.Empty());

			SECTION("Not exist remove")
			{
				BASIC_KV_FOR_EACH
				{
					ASSERT_THROW(KeyNotFoundException, btree.Remove(p.first));
				}
			}

			SECTION("Add lot")
			{
				ADD_LOT_FROM(completeKeyValueArray);
			}
		}
	}
#undef BASIC_KV_FOR_EACH

	SECTION("Count less than BtreeOrder cons")
	{
		using Btr = Btree<10, string, string>;
		Btr btree{ lessThan, basicKeyValueArray };
	}

	SECTION("Random add and remove")
	{
		using ::std::cout;
		using ::std::random_device;
		using ::std::mt19937;
		using Btr = Btree<4, string, string>;
		for (auto i = 0; i < 100; ++i)
		{
			Btr btree(lessThan);
			random_device rd;
			mt19937 g(rd());
			shuffle(completeKeyValueArray.begin(), completeKeyValueArray.end(), g);
			
			for (auto& p : completeKeyValueArray)
			{
				btree.Add(p);
				try
				{
					ASSERT(btree.ContainsKey(p.first));
				}
				catch (AssertionFailure const& e)
				{
					cout << "Shuffled array: ";
					for (auto& e : completeKeyValueArray)
					{
						cout << e.first << " ";
					}
					cout << endl;

					throw e;
				}
			}

			for (auto& p : completeKeyValueArray)
			{
				btree.Remove(p.first);
				try
				{
					ASSERT(!btree.ContainsKey(p.first));
				}
				catch (AssertionFailure const& e)
				{
					cout << "Shuffled array: ";
					for (auto& e : completeKeyValueArray)
					{
						cout << e.first << " ";
					}
					cout << endl;

					throw e;
				}
			}
		}
		// check balance work or not?
		// 4 14 3 7 18 16 5 17 1 19 10 11 2 6 15 8 13 9 0 12
		// 1 5 18 11 2 0 9 17 4 10 16 13 8 6 7 19 15 12 3 14

		//auto kv1 = make_pair<string, string>("1", "a");
		//auto kv5 = make_pair<string, string>("5", "a");
		//auto kv18 = make_pair<string, string>("18", "a");
		//auto kv11 = make_pair<string, string>("11", "a");
		//auto kv2 = make_pair<string, string>("2", "a");
		//auto kv0 = make_pair<string, string>("0", "a");
		//auto kv9 = make_pair<string, string>("9", "a");
		//auto kv17 = make_pair<string, string>("17", "a");
		//auto kv4 = make_pair<string, string>("4", "a");
		//auto kv10 = make_pair<string, string>("10", "a");
		//auto kv16 = make_pair<string, string>("16", "a");
		//auto kv13 = make_pair<string, string>("13", "a");
		//auto kv8 = make_pair<string, string>("8", "a");
		//auto kv6 = make_pair<string, string>("6", "a");
		//auto kv7 = make_pair<string, string>("7", "a");
		//auto kv19 = make_pair<string, string>("19", "a");
		//auto kv15 = make_pair<string, string>("15", "a");
		//auto kv12 = make_pair<string, string>("12", "a");
		//auto kv3 = make_pair<string, string>("3", "a");
		//auto kv14 = make_pair<string, string>("14", "a");

		//array<decltype(kv0), 20> record
		//{
		//	kv1, kv5, kv18, kv11, kv2, kv0, kv9,
		//	kv17, kv4, kv10, kv16, kv13, kv8, kv6,
		//	kv7, kv19, kv15, kv12, kv3, kv14,
		//};

		//for (auto& e : record)
		//{
		//	btree.Add(e);
		//	if (!btree.ContainsKey(e.first))
		//	{
		//		D(e.first);
		//	}
		//	ASSERT(btree.ContainsKey(e.first));
		//}

		//for (auto& e : record)
		//{
		//	btree.Remove(e.first);
		//	if (btree.ContainsKey(e.first))
		//	{
		//		D(e.first);
		//	}
		//	ASSERT(!btree.ContainsKey(e.first));
		//}
	}
}

#undef ADD_LOT_FROM

TESTCASE("Int-Str btree test")
{
	auto kv0 = make_pair<int32_t, string>(0, "a");
	auto kv1 = make_pair<int32_t, string>(1, "b");
	auto kv2 = make_pair<int32_t, string>(2, "c");
	auto kv3 = make_pair<int32_t, string>(3, "d");
	auto kv4 = make_pair<int32_t, string>(4, "e");
	auto kv5 = make_pair<int32_t, string>(5, "f");
	auto kv6 = make_pair<int32_t, string>(6, "g");
	auto kv7 = make_pair<int32_t, string>(7, "h");
	auto kv8 = make_pair<int32_t, string>(8, "i");
	auto kv9 = make_pair<int32_t, string>(9, "j");
	auto kv10 = make_pair<int32_t, string>(10, "k");
	auto kv11 = make_pair<int32_t, string>(11, "l");
	auto kv12 = make_pair<int32_t, string>(12, "m");
	auto kv13 = make_pair<int32_t, string>(13, "n");
	auto kv14 = make_pair<int32_t, string>(14, "o");
	auto kv15 = make_pair<int32_t, string>(15, "p");
	auto kv16 = make_pair<int32_t, string>(16, "q");
	auto kv17 = make_pair<int32_t, string>(17, "r");
	auto kv18 = make_pair<int32_t, string>(18, "s");
	auto kv19 = make_pair<int32_t, string>(19, "t");

	array<pair<int32_t, string>, 9> basicKeyValueArray
	{
		kv0, kv1, kv2, kv3, kv4, kv5, kv6, kv7, kv8,
	};

	array<pair<int32_t, string>, 11> secondKeyValueArray
	{
		kv9, kv10, kv11, kv12, kv13, kv14, kv15, kv16, kv17, kv18, kv19,
	};

	array<pair<int32_t, string>, 20> completeKeyValueArray
	{
		kv0, kv1, kv2, kv3, kv4, kv5, kv6, kv7, kv8, kv9,
		kv10, kv11, kv12, kv13, kv14, kv15, kv16, kv17, kv18, kv19,
	};
	auto lessThan = [](int32_t const& a, int32_t const& b) { return a < b; };

	SECTION("Count more than BtreeOrder btree")
	{
		using Btr = Collections::Btree<4, int32_t, string>;
		Btr btree(lessThan, basicKeyValueArray);
	}
}

DEF_TEST_FUNC(btreeTest)

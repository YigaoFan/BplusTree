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
	using ValueType = MemoryMemo;
#define VALUE MemoryMemo()
	/*auto kv0 = make_pair<string, ValueType>("0", VALUE);
	auto kv1 = make_pair<string, ValueType>("1", VALUE);
	auto kv2 = make_pair<string, ValueType>("2", VALUE);
	auto kv3 = make_pair<string, ValueType>("3", VALUE);
	auto kv4 = make_pair<string, ValueType>("4", VALUE);
	auto kv5 = make_pair<string, ValueType>("5", VALUE);
	auto kv6 = make_pair<string, ValueType>("6", VALUE);
	auto kv7 = make_pair<string, ValueType>("7", VALUE);
	auto kv8 = make_pair<string, ValueType>("8", VALUE);
	auto kv9 = make_pair<string, ValueType>("9", VALUE);
	auto kv10 = make_pair<string, ValueType>("10", VALUE);
	auto kv11 = make_pair<string, ValueType>("11", VALUE);
	auto kv12 = make_pair<string, ValueType>("12", VALUE);
	auto kv13 = make_pair<string, ValueType>("13", VALUE);
	auto kv14 = make_pair<string, ValueType>("14", VALUE);
	auto kv15 = make_pair<string, ValueType>("15", VALUE);
	auto kv16 = make_pair<string, ValueType>("16", VALUE);
	auto kv17 = make_pair<string, ValueType>("17", VALUE);
	auto kv18 = make_pair<string, ValueType>("18", VALUE);
	auto kv19 = make_pair<string, ValueType>("19", VALUE);*/
	auto kv0 = make_pair<int32_t, string>(0, "Hello world, fanyigao");
	auto kv1 = make_pair<int32_t, string>(1, "Hello world, fanyigao");
	auto kv2 = make_pair<int32_t, string>(2, "Hello world, fanyigao");
	auto kv3 = make_pair<int32_t, string>(3, "Hello world, fanyigao");
	auto kv4 = make_pair<int32_t, string>(4, "Hello world, fanyigao");
	auto kv5 = make_pair<int32_t, string>(5, "Hello world, fanyigao");
	auto kv6 = make_pair<int32_t, string>(6, "Hello world, fanyigao");
	auto kv7 = make_pair<int32_t, string>(7, "Hello world, fanyigao");
	auto kv8 = make_pair<int32_t, string>(8, "Hello world, fanyigao");
	auto kv9 = make_pair<int32_t, string>(9, "Hello world, fanyigao");
	auto kv10 = make_pair<int32_t, string>(10, "Hello world, fanyigao");
	auto kv11 = make_pair<int32_t, string>(11, "Hello world, fanyigao");
	auto kv12 = make_pair<int32_t, string>(12, "Hello world, fanyigao");
	auto kv13 = make_pair<int32_t, string>(13, "Hello world, fanyigao");
	auto kv14 = make_pair<int32_t, string>(14, "Hello world, fanyigao");
	auto kv15 = make_pair<int32_t, string>(15, "Hello world, fanyigao");
	auto kv16 = make_pair<int32_t, string>(16, "Hello world, fanyigao");
	auto kv17 = make_pair<int32_t, string>(17, "Hello world, fanyigao");
	auto kv18 = make_pair<int32_t, string>(18, "Hello world, fanyigao");
	auto kv19 = make_pair<int32_t, string>(19, "Hello world, fanyigao");
	/*auto kv0 = make_pair<int32_t, int>(0, 0);
	auto kv1 = make_pair<int32_t, int>(1, 0);
	auto kv2 = make_pair<int32_t, int>(2, 0);
	auto kv3 = make_pair<int32_t, int>(3, 0);
	auto kv4 = make_pair<int32_t, int>(4, 0);
	auto kv5 = make_pair<int32_t, int>(5, 0);
	auto kv6 = make_pair<int32_t, int>(6, 0);
	auto kv7 = make_pair<int32_t, int>(7, 0);
	auto kv8 = make_pair<int32_t, int>(8, 0);
	auto kv9 = make_pair<int32_t, int>(9, 0);
	auto kv10 = make_pair<int32_t, int>(10, 0);
	auto kv11 = make_pair<int32_t, int>(11, 0);
	auto kv12 = make_pair<int32_t, int>(12, 0);
	auto kv13 = make_pair<int32_t, int>(13, 0);
	auto kv14 = make_pair<int32_t, int>(14, 0);
	auto kv15 = make_pair<int32_t, int>(15, 0);
	auto kv16 = make_pair<int32_t, int>(16, 0);
	auto kv17 = make_pair<int32_t, int>(17, 0);
	auto kv18 = make_pair<int32_t, int>(18, 0);
	auto kv19 = make_pair<int32_t, int>(19, 0);*/

	array<decltype(kv0), 9> basicKeyValueArray 
	{
		kv0, kv1, kv2, kv3, kv4, kv5, kv6, kv7, kv8,
	};

	array<decltype(kv0), 4> secondKeyValueArray
	{
		kv9, kv10, kv11, kv12,// kv13,// kv14, kv15, kv16, kv17, //kv18, kv19,
	};

	array<decltype(kv0), 20> completeKeyValueArray
	{
		kv0, kv1, kv2, kv3, kv4, kv5, kv6, kv7, kv8, kv9,
		kv10, kv11, kv12, kv13, kv14, kv15, kv16, kv17, kv18, kv19,
	};

	//auto lessThan = [](string const& a, string const& b) { return a < b; };
	auto lessThan = [](int32_t const& a, int32_t const& b) { return a < b; };

	//SECTION("Duplicate tree cons", skip)
	//{
	//	using Btr = Collections::Btree<4, string, string>;
	//	array<pair<string, string>, 10> dupKeyValueArray
	//	{
	//		kv0, kv1, kv2, kv3, kv4,
	//		kv5, kv6, kv7, kv8, kv2,
	//	};
	//	ASSERT_THROW(DuplicateKeyException<string>, Btr(lessThan, dupKeyValueArray));
	//}

#define ADD_LOT_FROM(ARRAY)  for (auto& p : ARRAY) { btree.Add(p); ASSERT(btree.ContainsKey(p.first)); }
	//SECTION("Empty cons", skip)
	//{
	//	using Btr = Collections::Btree<4, string, ValueType>;
	//	Btr btree(lessThan);
	//	ASSERT(btree.Empty());

	//	SECTION("Add a lot")
	//	{
	//		ADD_LOT_FROM(completeKeyValueArray);
	//	}
	//}

	//using Btr = Collections::Btree<4, string, ValueType>;
	//{
	//	Btr btree(lessThan, basicKeyValueArray);
	//	//SECTION("Add a lot")
	//	{
	//		//ADD_LOT_FROM(secondKeyValueArray);
	//		for (auto& p : secondKeyValueArray)
	//		{
	//			btree.Add(p);
	//			btree.PrintTree();// TODO this arg can be anything which like cout
	//			cout << endl;
	//			ASSERT(btree.ContainsKey(p.first));
	//		}
	//	}
	//}

	SECTION("Count more than BtreeOrder btree")
	{
		using Btr = Collections::Btree<4, int32_t, string>;
		{
			for (auto i = 0; i < 100; ++i)
			{
				Btr btree(lessThan, basicKeyValueArray);
				//SECTION("Add a lot")
				{
					//ADD_LOT_FROM(secondKeyValueArray);
					for (auto& p : secondKeyValueArray)
					{
						btree.Add(p);
						//btree.PrintTree();// TODO this arg can be anything which like cout
						//cout << endl;
						ASSERT(btree.ContainsKey(p.first));
					}
				}
			}
		}

		::std::cout << "Add a lot end" << ::std::endl;


		//SECTION("Move cons")
		//{
		//	auto n = btree.Count();
		//	auto b = move(btree);
		//	ASSERT(btree.Count() == 0);
		//	ASSERT(b.Count() == n);
		//}

		//SECTION("Copy cons", skip)
		//{
		//	auto b = btree;
		//	ASSERT(b.Count() == btree.Count());
		//	b.Remove("3");
		//	ASSERT(!b.ContainsKey("3"));
		//	ASSERT(btree.ContainsKey("3"));
		//	//b.~Btr(); TODO here is problem
		//}

		/*SECTION("Normal function", skip)
		{
			auto keys = btree.Keys();
			MapContainers(CreateEnumerator(basicKeyValueArray), CreateEnumerator(keys), [&successCount](auto& e1, auto& e2)
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
		}*/



		//SECTION("Duplicate add", skip)
		//{
		//	BASIC_KV_FOR_EACH
		//	{
		//		ASSERT_THROW(DuplicateKeyException<string>, btree.Add(p));
		//	}
		//}

		//SECTION("Extreme remove", skip)
		//{
		//	BASIC_KV_FOR_EACH
		//	{
		//		btree.Remove(p.first);
		//		ASSERT(!btree.ContainsKey(p.first));
		//	}

		//	ASSERT(btree.Empty());

		//	SECTION("Not exist remove")
		//	{
		//		BASIC_KV_FOR_EACH
		//		{
		//			ASSERT_THROW(KeyNotFoundException, btree.Remove(p.first));
		//		}
		//	}

		//	SECTION("Add lot")
		//	{
		//		ADD_LOT_FROM(completeKeyValueArray);
		//	}
		//}
	}
#undef BASIC_KV_FOR_EACH

	//SECTION("Count less than BtreeOrder cons", skip)
	//{
	//	using Btr = Btree<10, string, string>;
	//	Btr btree{ lessThan, basicKeyValueArray };
	//}

	//SECTION("Random add and remove", skip)
	//{
	//	using ::std::cout;
	//	using ::std::random_device;
	//	using ::std::mt19937;
	//	for (auto i = 0; i < 10; ++i)
	//	{
	//		using Btr = Btree<4, string, string>;
	//		Btr btree(lessThan);
	//		random_device rd;
	//		mt19937 g(rd());
	//		shuffle(completeKeyValueArray.begin(), completeKeyValueArray.end(), g);
	//		cout << "Shuffled array: ";
	//		for (auto& e : completeKeyValueArray)
	//		{
	//			cout << e.first << " ";
	//		}
	//		cout << endl;

	//		ADD_LOT_FROM(completeKeyValueArray);

	//		for (auto& e : completeKeyValueArray)
	//		{
	//			btree.Remove(e.first);
	//			ASSERT(!btree.ContainsKey(e.first));
	//		}
	//	}
	//	// check balance work or not?
	//	// 4 14 3 7 18 16 5 17 1 19 10 11 2 6 15 8 13 9 0 12
	//	// 1 5 18 11 2 0 9 17 4 10 16 13 8 6 7 19 15 12 3 14

	//	//auto kv1 = make_pair<string, string>("1", "a");
	//	//auto kv5 = make_pair<string, string>("5", "a");
	//	//auto kv18 = make_pair<string, string>("18", "a");
	//	//auto kv11 = make_pair<string, string>("11", "a");
	//	//auto kv2 = make_pair<string, string>("2", "a");
	//	//auto kv0 = make_pair<string, string>("0", "a");
	//	//auto kv9 = make_pair<string, string>("9", "a");
	//	//auto kv17 = make_pair<string, string>("17", "a");
	//	//auto kv4 = make_pair<string, string>("4", "a");
	//	//auto kv10 = make_pair<string, string>("10", "a");
	//	//auto kv16 = make_pair<string, string>("16", "a");
	//	//auto kv13 = make_pair<string, string>("13", "a");
	//	//auto kv8 = make_pair<string, string>("8", "a");
	//	//auto kv6 = make_pair<string, string>("6", "a");
	//	//auto kv7 = make_pair<string, string>("7", "a");
	//	//auto kv19 = make_pair<string, string>("19", "a");
	//	//auto kv15 = make_pair<string, string>("15", "a");
	//	//auto kv12 = make_pair<string, string>("12", "a");
	//	//auto kv3 = make_pair<string, string>("3", "a");
	//	//auto kv14 = make_pair<string, string>("14", "a");

	//	//array<decltype(kv0), 20> record
	//	//{
	//	//	kv1, kv5, kv18, kv11, kv2, kv0, kv9,
	//	//	kv17, kv4, kv10, kv16, kv13, kv8, kv6,
	//	//	kv7, kv19, kv15, kv12, kv3, kv14,
	//	//};

	//	//for (auto& e : record)
	//	//{
	//	//	btree.Add(e);
	//	//	if (!btree.ContainsKey(e.first))
	//	//	{
	//	//		D(e.first);
	//	//	}
	//	//	ASSERT(btree.ContainsKey(e.first));
	//	//}

	//	//for (auto& e : record)
	//	//{
	//	//	btree.Remove(e.first);
	//	//	if (btree.ContainsKey(e.first))
	//	//	{
	//	//		D(e.first);
	//	//	}
	//	//	ASSERT(!btree.ContainsKey(e.first));
	//	//}
	//}
}

#undef ADD_LOT_FROM

//TESTCASE("Int-Str btree test")
//{
//	auto kv0 = make_pair<int32_t, string>(0, "a");
//	auto kv1 = make_pair<int32_t, string>(1, "b");
//	auto kv2 = make_pair<int32_t, string>(2, "c");
//	auto kv3 = make_pair<int32_t, string>(3, "d");
//	auto kv4 = make_pair<int32_t, string>(4, "e");
//	auto kv5 = make_pair<int32_t, string>(5, "f");
//	auto kv6 = make_pair<int32_t, string>(6, "g");
//	auto kv7 = make_pair<int32_t, string>(7, "h");
//	auto kv8 = make_pair<int32_t, string>(8, "i");
//	auto kv9 = make_pair<int32_t, string>(9, "j");
//	auto kv10 = make_pair<int32_t, string>(10, "k");
//	auto kv11 = make_pair<int32_t, string>(11, "l");
//	auto kv12 = make_pair<int32_t, string>(12, "m");
//	auto kv13 = make_pair<int32_t, string>(13, "n");
//	auto kv14 = make_pair<int32_t, string>(14, "o");
//	auto kv15 = make_pair<int32_t, string>(15, "p");
//	auto kv16 = make_pair<int32_t, string>(16, "q");
//	auto kv17 = make_pair<int32_t, string>(17, "r");
//	auto kv18 = make_pair<int32_t, string>(18, "s");
//	auto kv19 = make_pair<int32_t, string>(19, "t");
//
//	array<pair<int32_t, string>, 9> basicKeyValueArray
//	{
//		kv0, kv1, kv2, kv3, kv4, kv5, kv6, kv7, kv8,
//	};
//
//	array<pair<int32_t, string>, 11> secondKeyValueArray
//	{
//		kv9, kv10, kv11, kv12, kv13, kv14, kv15, kv16, kv17, kv18, kv19,
//	};
//
//	array<pair<int32_t, string>, 20> completeKeyValueArray
//	{
//		kv0, kv1, kv2, kv3, kv4, kv5, kv6, kv7, kv8, kv9,
//		kv10, kv11, kv12, kv13, kv14, kv15, kv16, kv17, kv18, kv19,
//	};
//	auto lessThan = [](int32_t const& a, int32_t const& b) { return a < b; };
//
//	SECTION("Count more than BtreeOrder btree")
//	{
//		using Btr = Collections::Btree<4, int32_t, string>;
//		Btr btree(lessThan, basicKeyValueArray);
//	}
//}

void btreeTest(bool isSkipped)
{
	if (!isSkipped)
	{
		allTest();
	}

	_tests_.~vector();
}

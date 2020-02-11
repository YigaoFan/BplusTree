#include <array>
#include <algorithm>
#include "../../TestFrame/FlyTest.hpp"
#include "../Btree.hpp"
#include "../Exception.hpp"
#include "../Enumerator.hpp"
#include "Util.hpp"

using ::std::array;
using ::std::move;
using namespace Collections;

TESTCASE("Btree test")
{
	auto kv0 = make_pair<string, string>("0", "a");
	auto kv1 = make_pair<string, string>("1", "b");
	auto kv2 = make_pair<string, string>("2", "c");
	auto kv3 = make_pair<string, string>("3", "d");
	auto kv4 = make_pair<string, string>("4", "e");
	auto kv5 = make_pair<string, string>("5", "f");
	auto kv6 = make_pair<string, string>("6", "g");
	auto kv7 = make_pair<string, string>("7", "h");
	auto kv8 = make_pair<string, string>("8", "i");
	auto kv9 = make_pair<string, string>("9", "j");
	auto kv10 = make_pair<string, string>("10", "k");
	auto kv11 = make_pair<string, string>("11", "l");
	auto kv12 = make_pair<string, string>("12", "m");
	auto kv13 = make_pair<string, string>("13", "n");
	auto kv14 = make_pair<string, string>("14", "o");
	auto kv15 = make_pair<string, string>("15", "p");
	auto kv16 = make_pair<string, string>("16", "q");
	auto kv17 = make_pair<string, string>("17", "r");
	auto kv18 = make_pair<string, string>("18", "s");
	auto kv19 = make_pair<string, string>("19", "t");

	array<pair<string, string>, 9> basicKeyValueArray 
	{
		kv0, kv1, kv2, kv3, kv4, kv5, kv6, kv7, kv8,
	};

	array<pair<string, string>, 11> secondKeyValueArray 
	{
		kv9, kv10, kv11, kv12, kv13, kv14, kv15, kv16, kv17, kv18, kv19,
	};

	array<pair<string, string>, 20> completeKeyValueArray 
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
			btree.~Btr();
			ASSERT(btree.ContainsKey("3"));
		}

		SECTION("Normal function")
		{
			auto keys = btree.Keys();
			MapContainers(CreateEnumerator(basicKeyValueArray), CreateEnumerator(keys), [](auto& e1, auto& e2)
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
#undef INSTANCE_FOR_EACH

	SECTION("Count less than BtreeOrder cons")
	{
		using Btr = Btree<10, string, string>;
		Btr btree{ lessThan, basicKeyValueArray };
	}

	SECTION("Random add")
	{
		// TODO
		// random_shuffle in <algorithm>
		// check balance work or not
		// random_shuffle(&a[0],&a[10]); wrong index
	}
}

void btreeTest()
{
	allTest();
	_tests_.~vector();
}

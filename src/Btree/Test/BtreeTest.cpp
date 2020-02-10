#include <array>
#include "../../TestFrame/FlyTest.hpp"
#include "../Btree.hpp"
#include "Exception.hpp"
using ::std::array;
using ::std::move;
using namespace Collections;

// TODO test construct 0 element, then add a lot of element

TESTCASE("Btree test")
{
	auto kv0 = make_pair<string, string>("1", "a");
	auto kv1 = make_pair<string, string>("2", "b");
	auto kv2 = make_pair<string, string>("3", "c");
	auto kv3 = make_pair<string, string>("4", "a");
	auto kv4 = make_pair<string, string>("5", "b");
	auto kv5 = make_pair<string, string>("6", "c");
	auto kv6 = make_pair<string, string>("7", "c");
	auto kv7 = make_pair<string, string>("8", "c");
	auto kv8 = make_pair<string, string>("9", "c");

	array<pair<string, string>, 9> keyValueArray 
	{
		kv0,
		kv1,
		kv2,
		kv3,
		kv4,
		kv5,
		kv6,
		kv7,
		kv8,
	};

	auto lessThan = [](string const& a, string const& b)
	{
		return a < b;
	};

	SECTION("Test count more than BtreeOrder btree")
	{
		using BTREE = Collections::Btree<4, string, string>;
		BTREE btree(lessThan, keyValueArray);

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
			ASSERT(b.ContainsKey("3"));
			btree.~BTREE();
			ASSERT(b.ContainsKey("4"));
		}

		SECTION("Contains Key")
		{
			
		}

		SECTION("Test normal function")
		{
			ASSERT(btree.GetValue(kv0.first) == kv0.second);
			ASSERT(btree.GetValue(kv8.first) == kv8.second);
			ASSERT_THROW(KeyNotFoundException, btree.GetValue("10"));

			btree.Add(make_pair("10", "d"));
			ASSERT(btree.GetValue("10") == "d");
			ASSERT(btree.ContainsKey("10"));

			btree.ModifyValue(kv0.first, "d");
			ASSERT(btree.GetValue(kv0.first) == "d");

			auto keys = btree.Keys();

			btree.Remove("10");
			ASSERT(!btree.ContainsKey("10"));
		}

		SECTION("Test add")
		{

		}

		SECTION("Test remove")
		{

		}
	}

	SECTION("Test count less than BtreeOrder construct")
	{
		using BTREE = Btree<10, string, string>;
		BTREE btree{ lessThan, keyValueArray };
	}
}

void btreeTest()
{
	allTest();
}

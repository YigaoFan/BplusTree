#include <array>
using std::array;
#include "FlyTest.hpp"
#include "Btree.hpp"
using namespace btree;

TESTCASE("Btree test") {
	auto kv0 = make_pair<string, string>("1", "a");
	auto kv1 = make_pair<string, string>("2", "b");
	auto kv2 = make_pair<string, string>("3", "c");
	auto kv3 = make_pair<string, string>("4", "a");
	auto kv4 = make_pair<string, string>("5", "b");
	auto kv5 = make_pair<string, string>("6", "c");
	auto kv6 = make_pair<string, string>("7", "c");
	auto kv7 = make_pair<string, string>("8", "c");
	auto kv8 = make_pair<string, string>("9", "c");

	array<pair<string, string>, 9> keyValueArray {
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

	function<bool(const string&, const string&)> lessThan = [] (const string& a, const string& b) {
		return a < b;
	};

	SECTION("Test count more than BtreeOrder btree") {
		using BTREE = Btree<4, string, string>;
		BTREE btree{ lessThan, keyValueArray };

		SECTION("Test move") {

		}

		SECTION("Test copy") {

		}

		SECTION("Test normal function") {
			ASSERT(btree.search(kv0.first) == kv0.second);
			ASSERT(btree.search(kv8.first) == kv8.second);
			ASSERT_THROW(runtime_error, btree.search("10"));

			btree.add(make_pair("10", "d")); // TODO
			ASSERT(btree.search("10") == "d");

			btree.modify(make_pair(kv0.first, "d"));
			ASSERT(btree.search(kv0.first) == "d");

//			auto keys = btree.explore();

//			btree.remove("10");
			ASSERT_THROW(runtime_error, btree.have("10"));
		}
	}

	SECTION("Test count less than BtreeOrder construct") {
		using BTREE = Btree<10, string, string>;
		BTREE btree{ lessThan, keyValueArray };
	}
}

void
btreeTest()
{
	allTest();
}

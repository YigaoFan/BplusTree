#include <array>
using std::array;
#include "../../TestFrame/FlyTest.hpp"
#include "../Btree.hpp"
using namespace Collections;

// TODO test construct 0 element, then add a lot of element

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
		using BTREE = Collections::Btree<4, string, string>;
		BTREE btree(lessThan, keyValueArray);

		SECTION("Test move") {

		}

		SECTION("Test copy") {

		}

		SECTION("Test normal function") {
			ASSERT(btree.Search(kv0.first) == kv0.second);
			ASSERT(btree.Search(kv8.first) == kv8.second);
			ASSERT_THROW(runtime_error, btree.Search("10"));

			btree.Add(make_pair("10", "d"));
			ASSERT(btree.Search("10") == "d");
			ASSERT(btree.Have("10"));

			btree.Modify(make_pair(kv0.first, "d"));
			ASSERT(btree.Search(kv0.first) == "d");

			auto keys = btree.Explore();

			btree.Remove("10");
			ASSERT_THROW(runtime_error, btree.Have("10"));
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

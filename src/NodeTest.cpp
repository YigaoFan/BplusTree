#include <array>
#include <memory>
#include "Utility.hpp"
using std::array;
using std::make_shared;
#include "FlyTest.hpp"
#include "MiddleNode.hpp"
#include "LeafNode.hpp"
using namespace btree;

TESTCASE("Middle node test") {
	using MIDDLE = MiddleNode<string, string, 3>;

}

TESTCASE("Leaf node test") {
	using LEAF = LeafNode<string, string, 3>;
	auto kv0 = make_pair<string, string>("1", "a");
	auto kv1 = make_pair<string, string>("2", "b");
	auto kv2 = make_pair<string, string>("3", "c");
	auto kv3 = make_pair<string, string>("4", "a");
	auto kv4 = make_pair<string, string>("5", "b");
	auto kv5 = make_pair<string, string>("6", "c");

	array<pair<string, string>, 3> keyValueArray1 {
		kv0,
		kv1,
		kv2,
	};
	array<pair<string, string>, 3> keyValueArray2 {
		kv3,
		kv4,
		kv5,
	};

	function<bool(const string&, const string&)> compareFunction = [] (const string& a, const string& b) {
		return a > b;
	};

	LEAF leaf1{ keyValueArray1.begin(), keyValueArray1.end(), make_shared<decltype(compareFunction)>(compareFunction) };
	LEAF leaf2{ keyValueArray2.begin(), keyValueArray2.end(), make_shared<decltype(compareFunction)>(compareFunction) };
	leaf1.nextLeaf(leaf2);

	SECTION("Test no change function") {
		ASSERT(!leaf1.Middle);
		ASSERT(!leaf2.Middle);
		ASSERT(leaf1.father() == nullptr);
		ASSERT(leaf2.father() == nullptr);
		ASSERT(leaf1.nextLeaf() == &leaf2);
		ASSERT(leaf2.nextLeaf() == nullptr);
		ASSERT(leaf1[0] == kv0);
		ASSERT(leaf1[1] == kv1);
		ASSERT(leaf1[2] == kv2);
		ASSERT(leaf1["1"] == kv0.second);
		ASSERT(leaf1["2"] == kv1.second);
		ASSERT(leaf1["3"] == kv2.second);
		ASSERT(leaf1.maxKey() == "3");
		vector<string> ks{ "1", "2", "3" };
		ASSERT(ks == leaf1.allKey());
	}

	SECTION("Test copy") {
		LEAF c{ leaf1, nullptr };

		ASSERT(!c.Middle);
		ASSERT(c.allKey() == leaf1.allKey());
		leaf1.remove("1");
		ASSERT(!leaf1.have("1"));
		ASSERT(c.have("1"));
	}

	SECTION("Test move") {
		LEAF c{ std::move(leaf1) };
		ASSERT(leaf1.childCount() == 0);
		ASSERT(!c.Middle);
		ASSERT(!leaf1.have("1"));
		ASSERT(c.have("1"));
		ASSERT(c[0] == kv0);
	}

	SECTION("Test add") {
//		leaf1.add()
	}

	SECTION("Test remove") {

	}
}

void
nodeTest()
{
	allTest();
}

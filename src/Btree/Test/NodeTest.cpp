#include <array>
#include <memory>
#include "Utility.hpp"
using std::array;
using std::make_shared;
#include "FlyTest.hpp"
#include "../MiddleNode.hpp"
#include "../LeafNode.hpp"
using namespace btree;

static auto kv0 = make_pair<string, string>("1", "a");
static auto kv1 = make_pair<string, string>("2", "b");
static auto kv2 = make_pair<string, string>("3", "c");
static auto kv3 = make_pair<string, string>("4", "a");
static auto kv4 = make_pair<string, string>("5", "b");
static auto kv5 = make_pair<string, string>("6", "c");
static auto kv6 = make_pair<string, string>("7", "c");
static auto kv7 = make_pair<string, string>("8", "c");
static auto kv8 = make_pair<string, string>("9", "c");

static array<pair<string, string>, 3> keyValueArray1 {
	kv0,
	kv1,
	kv2,
};
static array<pair<string, string>, 3> keyValueArray2 {
	kv3,
	kv4,
	kv5,
};
static array<pair<string, string>, 3> keyValueArray3 {
	kv6,
	kv7,
	kv8,
};
static function<bool(const string&, const string&)> lessThan = [] (const string& a, const string& b) {
	return a < b;
};

TESTCASE("Middle node test") {
	using LEAF   = LeafNode  <string, string, 3>;
	using MIDDLE = MiddleNode<string, string, 3>;
	using BASE   = NodeBase  <string, string, 3>;

	auto makeLeaf = [] (auto& kvArray, auto& func) {
		return make_unique<LEAF>(kvArray.begin(), kvArray.end(), make_shared<decltype(lessThan)>(func));
	};

	auto leaf0 = makeLeaf(keyValueArray1, lessThan);
	auto leaf1 = makeLeaf(keyValueArray2, lessThan);
	auto leaf2 = makeLeaf(keyValueArray3, lessThan);

    auto kl0 = make_pair<string, unique_ptr<LEAF>>(leaf0->maxKey(), std::move(leaf0));
    auto kl1 = make_pair<string, unique_ptr<LEAF>>(leaf1->maxKey(), std::move(leaf1));
    auto kl2 = make_pair<string, unique_ptr<LEAF>>(leaf2->maxKey(), std::move(leaf2));
	array<decltype(kl0), 3> leafArray {
		std::move(kl0),
		std::move(kl1),
		std::move(kl2),
	};

	MIDDLE middle{ leafArray.begin(), leafArray.end(), make_shared<decltype(lessThan)>(lessThan) };

	SECTION("Test no change function") {
		// ASSERT(middle.minSon() == leaf0.get());
		// ASSERT(middle.maxSon() == leaf2.get());
		// ASSERT(middle[kl0.first] == leaf0.get());
		// ASSERT(middle[kl1.first] == leaf1.get());
		// ASSERT(middle[kl2.first] == leaf2.get());
		ASSERT(middle[0].first == kl0.first);
		ASSERT(middle[0].second == static_cast<BASE*>(kl0.second.get()));
		ASSERT(middle[1].first == kl1.first);
		ASSERT(middle[1].second == static_cast<BASE*>(kl1.second.get()));
		ASSERT(middle[2].first == kl2.first);
		ASSERT(middle[2].second == static_cast<BASE*>(kl2.second.get()));
		// TODO test leaf nextLeaf()
	}
}

TESTCASE("Leaf node test") {
	using LEAF = LeafNode<string, string, 3>;

	LEAF leaf1{ keyValueArray1.begin(), keyValueArray1.end(), make_shared<decltype(lessThan)>(lessThan) };
	LEAF leaf2{ keyValueArray2.begin(), keyValueArray2.end(), make_shared<decltype(lessThan)>(lessThan) };
	leaf1.nextLeaf(&leaf2);

	SECTION("Test no change function") {
		ASSERT(!leaf1.Middle);
		ASSERT(!leaf2.Middle);
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
		// ASSERT(leaf1.full());
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
		leaf1.remove("1");
		ASSERT(leaf1.childCount() == 2);
		ASSERT(!leaf1.have("1"));
		// if (!leaf1.full()) {
		// 	ASSERT(leaf1.add(make_pair<string, string>("4", "d")));
		// }
		ASSERT(leaf1.have("4"));
		// ASSERT(leaf1.full());

		leaf1.remove("2");
		ASSERT(leaf1.childCount() == 2);
		ASSERT(!leaf1.have("2"));
		// if (!leaf1.full()) {
		// 	ASSERT(!leaf1.add(make_pair<string, string>("2", "b")));
		// }
	}

	SECTION("Test remove") {
		leaf1.remove(kv0.first);
		ASSERT(leaf1.childCount() == 2);
		leaf1.remove(kv1.first);
		ASSERT(leaf1.childCount() == 1);
		leaf1.remove(kv2.first);
		ASSERT(leaf1.childCount() == 0);
		// ASSERT(leaf1.empty());
	}
}

void
nodeTest()
{
	allTest();
}

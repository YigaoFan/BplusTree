#include <string>
#include <array>
#include <utility>
#include <cassert>
#include <iostream>
#include "FlyTest.hpp"
#include "Elements.hpp"
#include "Utility.hpp"
using namespace btree;
using std::string;
using std::array;
using std::make_pair;
using std::pair;
using std::cout;
using std::endl;

class DummyNode {
};

TESTCASE("Element test") {
	function<bool(const string&, const string&)> lessThan = [] (const string& a, const string& b) {
		return a < b;
	};
	using ELE = Elements<string, string, 3, DummyNode>;
	using std::make_shared;

	SECTION("Test leaf Element construct") {
		array<pair<string, string>, 3> keyValueArray {
			make_pair("1", "a"),
			make_pair("2", "b"),
			make_pair("3", "c"),
		};
		ELE leafEle{keyValueArray.begin(), keyValueArray.end(), make_shared<decltype(lessThan)>(lessThan)};

		ASSERT(leafEle.count() == 3);
		ASSERT(leafEle.full());
		ASSERT(leafEle.have("1"));
		ASSERT(leafEle.have("2"));
		ASSERT(leafEle.have("3"));
		ASSERT(!leafEle.have("4"));
		ASSERT(leafEle.rightMostKey() == "3");
		ASSERT(ELE::value(leafEle["1"]) == "a");
		ASSERT(ELE::value(leafEle["2"]) == "b");
		ASSERT(ELE::value(leafEle["3"]) == "c");
		ASSERT(ELE::value(leafEle[0].second) == "a");
		ASSERT(ELE::value(leafEle[1].second) == "b");
		ASSERT(ELE::value(leafEle[2].second) == "c");
		ASSERT(leafEle[0].first == "1");
		ASSERT(leafEle[1].first == "2");
		ASSERT(leafEle[2].first == "3");
		auto&& keys = leafEle.allKey();
		ASSERT(keys[0] == "1");
		ASSERT(keys[1] == "2");
		ASSERT(keys[2] == "3");

		SECTION("Test copy") {
			ELE b{leafEle};

			ASSERT(b.count() == 3);
			ASSERT(b.full());
			ASSERT(!b.remove("1"));
			ASSERT(!b.have("1"));
			ASSERT(leafEle.have("1"));
		}

		SECTION("Test move") {
			ELE b{std::move(leafEle)};

			ASSERT(b.count() == 3);
			ASSERT(b.full());
			ASSERT(!b.remove("1"));
			ASSERT(!b.have("1"));
			ASSERT(!leafEle.have("1"));
		}

		SECTION("Test remove") {
			ASSERT(!leafEle.remove("2"));
			ASSERT(!leafEle.have("2"));
			ASSERT(leafEle.count() == 2);
			ASSERT(!leafEle.remove("1"));
			ASSERT(!leafEle.have("1"));
			ASSERT(leafEle.count() == 1);
			ASSERT(leafEle.remove("3"));
			ASSERT(!leafEle.have("3"));
			ASSERT(leafEle.count() == 0);

			SECTION("Test append and insert") {
				leafEle.append(make_pair(string{"3"}, string{"c"}));
				ASSERT_THROW(runtime_error, leafEle.insert(make_pair(string{"4"}, string{"d"})));
				ASSERT_THROW(runtime_error, leafEle.insert(make_pair(string{"3"}, string{"c"})));
				leafEle.insert(make_pair(string{"2"}, string{"b"}));
				ASSERT(leafEle.count() == 2);
				ASSERT(leafEle.rightMostKey() == "3");
				ASSERT(ELE::value(leafEle["2"]) == "b");
				ASSERT(ELE::value(leafEle["3"]) == "c");
			}
		}

		SECTION("Test full append") {
			ASSERT_THROW(runtime_error, leafEle.append(make_pair(string{"4"}, string{"d"})));
		}

		SECTION("Test exchange") {
			ASSERT(leafEle.full());
			auto&& max = leafEle.exchangeMax(make_pair("4", "d"));
			ASSERT(max.first == "3");
			ASSERT(max.second == "c");
			ASSERT(ELE::value(leafEle["4"]) == "d");
		}

		SECTION("Test iterator") {
			for (auto& e : leafEle) {
			}
		}

	}

	SECTION("Test middle Element construct") {
		auto* n1 = new DummyNode();
		auto* n2 = new DummyNode();
		auto* n3 = new DummyNode();

		array<pair<string, DummyNode*>, 3> keyValueArray {
			make_pair("1", n1),
			make_pair("2", n2),
			make_pair("3", n3),
		};

		ELE middle{keyValueArray.begin(), keyValueArray.end(), make_shared<decltype(lessThan)>(lessThan)};

		ASSERT(middle.count() == 3);
		ASSERT(middle.full());
		ASSERT(middle.have("1"));
		ASSERT(middle.have("2"));
		ASSERT(middle.have("3"));
		ASSERT(!middle.have("4"));
		ASSERT(middle.rightMostKey() == "3");
		ASSERT(ELE::ptr(middle["1"]) == n1);
		ASSERT(ELE::ptr(middle["2"]) == n2);
		ASSERT(ELE::ptr(middle["3"]) == n3);
		auto&& keys = middle.allKey();
		ASSERT(keys[0] == "1");
		ASSERT(keys[1] == "2");
		ASSERT(keys[2] == "3");
		ASSERT(middle.ptrOfMin() == n1);
		ASSERT(middle.ptrOfMax() == n3);
		ASSERT(ELE::ptr(middle[0].second) == n1);
		ASSERT(ELE::ptr(middle[1].second) == n2);
		ASSERT(ELE::ptr(middle[2].second) == n3);
		ASSERT(middle[0].first == "1");
		ASSERT(middle[1].first == "2");
		ASSERT(middle[2].first == "3");

		SECTION("Test copy") {
			ELE b{middle};

			ASSERT(ELE::ptr(b["1"]) != ELE::ptr(middle["1"]));
			ASSERT(ELE::ptr(b["2"]) != ELE::ptr(middle["2"]));
			ASSERT(ELE::ptr(b["3"]) != ELE::ptr(middle["3"]));
		}

		SECTION("Test move") {
			ELE m{std::move(middle)};
			ASSERT(middle.count() == 0);
			ASSERT(m.count() == 3);
			ASSERT(ELE::ptr(m["1"]) == n1);
			ASSERT(ELE::ptr(m["2"]) == n2);
			ASSERT(ELE::ptr(m["3"]) == n3);
			ASSERT_THROW(runtime_error, ELE::ptr(middle["1"]));
			ASSERT_THROW(runtime_error, ELE::ptr(middle["2"]));
			ASSERT_THROW(runtime_error, ELE::ptr(middle["3"]));
		}
	}
}

void
elementTest()
{
	allTest();
}

#include <string>
#include <array>
#include <utility>
#include <cassert>
#include <iostream>
#define BTREE_DEBUG
#include "Elements.hpp"
#include "Utility.hpp"
using namespace btree;
using std::string;
using std::array;
using std::make_pair;
using std::pair;
using std::cout;
using std::endl;

class Node {
	// for test only
};

static array<pair<string, string>, 3> keyValueArray {
	make_pair("1", "a"),
    make_pair("2", "b"),
    make_pair("3", "c"),
};

// who should save the compare function? Btree or Elements?
static std::function<bool(string, string)> compareFunction = [] (string a, string b) {
	return a > b;
};

static Elements<string, string, 3, Node> elements{keyValueArray.cbegin(), keyValueArray.cend(), compareFunction};

static
void
testCopy()
{
	Elements<string, string , 3, Node> anotherOne{elements, compareFunction};
	// what's the rule for copy constructor? only one arg, other should have default value?
}

static
void
testBound()
{
	EXPECT_EQ("3", elements.rightmostKey());
}

static
void
testCount()
{
	EXPECT_EQ(3, elements.count());
}

static
void
testFull()
{
	EXPECT_EQ(true, elements.full());
}

static
void
testExistence()
{
	EXPECT_EQ(true, elements.have("1"));
	EXPECT_EQ(false, elements.have("4"));
}

static
void
testAllKey()
{
	auto&& keys = elements.allKey();
	EXPECT_EQ("1", keys[0]);
	EXPECT_EQ("2", keys[1]);
	EXPECT_EQ("3", keys[2]);
}

static
void
testRemove()
{
	EXPECT_EQ(false, elements.remove("1"));
	auto&& keys = elements.allKey();
	EXPECT_EQ(2, keys.size());
	EXPECT_EQ("2", keys[0]);
	EXPECT_EQ("3", keys[1]);

	EXPECT_EQ(true, elements.remove("3"));
	auto&& ks = elements.allKey();
	EXPECT_EQ(1, ks.size());
	EXPECT_EQ("2", ks[0]);

	EXPECT_EQ(true, elements.remove("2"));
	auto&& k = elements.allKey();
	EXPECT_EQ(0, k.size());

	EXPECT_EQ(false, elements.full());
}

static
void
testAdd()
{
	// insert will only insert, append will append
	EXPECT_EQ(true, elements.append(make_pair(string{"4"}, string{"d"})));
	auto&& keys = elements.allKey();
	EXPECT_EQ(1, keys.size());
	EXPECT_EQ("4", keys[0]);

	EXPECT_EQ(false, elements.insert(make_pair(string{"1"}, string{"a"})));
	auto&& ks = elements.allKey();
	EXPECT_EQ(2, ks.size());
	EXPECT_EQ("1", ks[0]);
	EXPECT_EQ("4", ks[1]);

	EXPECT_EQ(false, elements.insert(make_pair(string{"2"}, string{"b"})));
	auto&& k = elements.allKey();
	EXPECT_EQ(3, k.size());
	EXPECT_EQ("1", k[0]);
	EXPECT_EQ("2", k[1]);
	EXPECT_EQ("4", k[2]);
}

static
void
testOperatorRelated()
{
	auto& result = elements["4"];
	auto& v = Elements<string, string, 3, Node>::value(result);
	EXPECT_EQ(string{"d"}, v);

	try {
		auto& result = elements["3"];
	} catch (std::exception e) {
//		cout << "Catched!" << endl;
	}
}

static
void
testSqueeze()
{
	if (elements.full()) {
		cout << "Start squeeze!" << endl;
		auto&& result = elements.squeezeOne(make_pair(string{"3"}, string{"c"}));
		EXPECT_EQ("4", result.first);
		EXPECT_EQ(true, elements.have("3"));
	}

}

static
void
ElementsTest()
{
	testCopy();
	testBound();
	testCount();
	testFull();
	testExistence();
	testAllKey();
	testRemove();
	testAdd();
	testOperatorRelated();
	testSqueeze();
}

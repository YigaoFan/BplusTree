#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <array>
#include <variant>
#include "Utility.hpp"

//#define BTREE_DEBUG
//#include "Btree.hpp"
//using namespace btree;
#include "TestSuite.cpp"
using std::string;
using std::cout;
using std::vector;
using std::endl;
using std::shared_ptr;
using std::make_shared;
using std::ostream;
using std::pair;
using std::array;
using std::make_pair;


//array<pair<int, string>, 7> keyValueArray {
//    make_pair(2, "b"),
//    make_pair(1, "a"),
//    make_pair(3, "b"),
//    make_pair(4, "d"),
//    make_pair(5, "f"),
//    make_pair(6, "g"),
//    make_pair(7, "h"),
//};

//bool
//comp(const int a, const int b)
//{
//    return a < b;
//}

//static
//void
//test()
//{
//    LOG("Debug Info: ")
//    auto b{ Btree<int, string, 3>(comp, keyValueArray)};
//    auto a = b;
//    auto c = std::move(a);

//    LOG("Console: ")
//    for (auto& e : b.explore()) {
//        cout << e << endl;
//    }

//    LOG(b.have(1));
//    LOG(b.search(1));
//    LOG(b.add({6, "g"}));
//    LOG(b.modify({1, "k"}));
//    b.remove(6);
//}

int
main()
{
	allTest();
    return 0;
}
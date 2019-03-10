#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <array>
#include <variant>

#define BTREE_DEBUG
#include "Btree.hpp"

using namespace btree;
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

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual)                           \
    do {                                                                   \
        ++test_count;                                                      \
        if (equality) {                                                    \
            ++test_pass;                                                   \
        } else {                                                           \
            std::cerr << __FILE__ <<" : " << __LINE__ << ": "<< "expect: " \
            << expect << " actual: "<< actual << std::endl;                \
            main_ret = 1;                                                  \
        }                                                                  \
    } while(0)

#define EXPECT_EQ_STR(expect, actual) \
    EXPECT_EQ_BASE((expect) == (actual), expect, actual)

array<pair<int, string>, 5> key_value_array {
    make_pair(2, "b"),
    make_pair(1, "a"),
    make_pair(3, "b"),
    make_pair(4, "d"),
    make_pair(5, "f"),
};

bool
comp(const int a, const int b)
{
    return a < b;
}

static
void
test()
{
    LOG("Debug Info: ")
    auto b{ Btree<int, string, 3>(comp, key_value_array)};
    LOG("Console: ")
    for (auto& e : b.explore()) {
        cout << e << endl;
    }

    LOG(b.have(1));
}

int
main()
{

    test();

//    cout << b.have(3) << endl;
//    EXPECT_EQ_STR(string("b"), b.search(2));
    return 0;
}
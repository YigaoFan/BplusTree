#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <array>
#include <variant>
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

//#define NDEBUG
int main()
{
    auto comp = [](const int a, const int b) { return a < b; };
    array<pair<int, string>, 3> key_value_array{
        make_pair<int, string>(2, "b"),
        make_pair<int, string>(1, "a"),
        make_pair<int, string>(3, "b"),
    };

    auto b{ Btree<int, string, 3>::Btree(comp, key_value_array) };
    std::variant<int, unique_ptr<string>> a;
    auto i = 1;
    a = i;
    a = std::make_unique<string>("Hello");
    std::cout << *std::get<unique_ptr<string>>(a) << std::endl;
    return 0;
}
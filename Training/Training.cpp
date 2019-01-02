// Training.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include "Btree.h"

using namespace btree;
using std::string;
using std::cout;
using std::vector;
using std::endl;
using std::shared_ptr;
using std::make_shared;
using std::ostream;
using std::pair;

int main()
{
    auto compare = [](const int a, const int b) { return a < b; };
    // now is 3-order Btree

    auto b = Btree<int, string, decltype(compare), 3>(compare, { {1, "a"}, {2, "b"}, {3, "c"}, });
    return 0;
}

// Training.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <array>
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
using std::array;
using std::make_pair;

//#define NDEBUG
int main()
{
    auto compa = [](const int a, const int b) { return a < b; };
    array<pair<int, string>, 3> key_value_array{
        make_pair<int, string>(1, "a")/* { 1, "a" }*/, // smart compiler? 0th element must like this
    };
    auto b{ Btree<int, string, 3>::Btree(compa, key_value_array) };
    return 0;
}

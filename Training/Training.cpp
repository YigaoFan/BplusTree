// Training.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "Btree.h"

using std::string;
using std::cout;
using std::vector;
using std::endl;
using std::shared_ptr;
using std::make_shared;
using std::ostream;
using btree::Btree;

int main()
{
    // now is 3-order Btree
    auto b = Btree();
    cout << b << endl;
    b.root.type == leaf;
    b.root.value == 2;
    return 0;
}

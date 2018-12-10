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

int main() 
{
	// now is 3-order Btree
	auto b = Btree({ 2, 1, /*3, 4, 5, 6, 7, */});
	std::list<int> i = {};
	cout << b << endl;
	return 0;
}

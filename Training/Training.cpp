// Training.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
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
	auto b = Btree();
	cout << b << endl;
	return 0;
}

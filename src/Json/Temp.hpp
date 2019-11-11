#pragma once
#include <string>
using std::string;

struct A {
	string a;
	int b;
	bool c;
};
auto [a, b, c, ] =  A{ };
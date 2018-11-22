#pragma once
#include <iostream>

struct Node {
	Node* _nextBrother;
	size_t childCount;
};
class Btree {
	friend std::ostream& operator<<(std::ostream& os, const Btree& item);
private:
	Node root = {
		nullptr,
		1,
	};

public:
	Btree();
	~Btree();
};




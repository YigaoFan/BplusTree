#pragma once
#ifndef NODE_H
#define NODE_H

#include <vector>
using std::vector;
typedef int ValueType;

class Node;
struct Ele
{
	ValueType childValueLowBound = 0;
	Node* child = nullptr;
};

class Node {
public:
	Node();
	~Node();
	vector<Ele> getVectorOfEles();

private:
	vector<Ele> eles(3);
	Node* nextBrother;
	decltype(eles.size()) elesCount;
};

#endif NODE_H

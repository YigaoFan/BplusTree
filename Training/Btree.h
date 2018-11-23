#pragma once
#ifndef BTREE_H
#define BTREE_H
#include <iostream>
#include <vector>
using std::vector;
struct Node;
typedef bool(*predicateFunc)(const Node&);

struct Node {
	Node* _1stChild;
	ValueType _2ndLowBound;
	Node* _2ndChild;
	ValueType _3rdLowBound;
	Node* _3rdChild;

	size_t childCount;
	Node* _nextBrother;
	// todo: get the iterator of the children of this Node
};

class Btree {
	friend std::ostream& operator<<(std::ostream& os, const Btree& item);
private:
	Node root = {
		nullptr,
		1,
	};
	void adjust();

public:
	Btree();
	~Btree();
	//Btree(ValueType e1, ValueType e2, ...);
	vector<Node*> find(predicateFunc func);
	int insert(ValueType e);
};



#endif NODE_H
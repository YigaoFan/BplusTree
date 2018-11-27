#pragma once
#ifndef BTREE_H
#define BTREE_H
#include <iostream>
#include <vector>
#include <functional> // for lambda
#include <initializer_list>
#include "Node.h"

using std::vector;
using std::initializer_list;

class Btree {
    friend std::ostream& operator<<(std::ostream& os, const Btree& item);
private:
	Node root{LEAF_NODE};
    void adjust();

public:
    Btree();
	Btree(initializer_list<KeyType> valueList);
    ~Btree();
    Node* find(PredicateFunc func);
    int insert(KeyType e);
};



#endif

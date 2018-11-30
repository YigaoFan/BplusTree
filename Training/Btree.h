#pragma once
#ifndef BTREE_H
#define BTREE_H
#include <iostream>
#include <vector>
#include <functional> // for lambda
#include <initializer_list>
#include <memory>
#include "Node.h"

using std::vector;
using std::initializer_list;
using std::shared_ptr;
using std::make_shared;

class Btree {
    friend std::ostream& operator<<(std::ostream& os, const Btree& item);
private:
	shared_ptr<Node> root = make_shared<Node>(LEAF_NODE);
    void adjust();

public:
    Btree();
	Btree(initializer_list<KeyType> valueList);
    ~Btree();
	shared_ptr<Node> find(PredicateFunc func);
    int insert(KeyType e);
};



#endif

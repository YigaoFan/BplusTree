#pragma once
#ifndef BTREE_H
#define BTREE_H
#include <iostream>
#include <vector>
#include "Node.h"
using std::vector;

class Btree {
    friend std::ostream& operator<<(std::ostream& os, const Btree& item);
private:
    Node *root;
    void adjust();

public:
    Btree();
    ~Btree();
    //Btree(ValueType e1, ValueType e2, ...);
    vector<Node*> find(predicateFunc func);
    int insert(ValueType e);
};



#endif

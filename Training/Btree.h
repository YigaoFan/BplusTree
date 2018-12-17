#pragma once
#ifndef BTREE_H
#define BTREE_H
#include <iostream> // for cin, cout
#include <vector> // for vector
#include <functional> // for lambda
#include <initializer_list> // for initializer_list
#include <memory> // for shared_ptr
#include <utility> // for pair
#include "Node.h"

// Should I think to setup a namespace
using std::vector;
using std::initializer_list;
using std::shared_ptr;
using std::make_shared;
using std::pair;

template <typename Key, typename Value, unsigned BtreeOrder>
class Btree {
private:
	typedef Node<Key, Value, BtreeOrder> node_type;
	shared_ptr<node_type> root_ = make_shared<node_type>(leaf);
	void adjust();
	bool check(Key);
	Value& traverse();
	shared_ptr<node_type> getFitLeafBack(typename node_type::PredicateFunc func);

public:
    Btree(initializer_list<pair<Key, Value>>);
    ~Btree();
    Value search(Key);
    int add(pair<Key, Value>);
    int modify(pair<Key, Value>);
    const vector<Key> explore();
    int remove(Key);
};
#endif

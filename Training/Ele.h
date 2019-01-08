#pragma once
#include "Node.h"
using namespace btree;

template <typename Key, typename Value>
class Ele {
public:
    Ele();
    ~Ele();

    virtual Key& key();
};


template <typename Key, 
          typename Value>
class ValueEle : public Ele<Key, Value> {
private:
    std::pair<Key, Value> leaf_;
public:
    Key& key() override
    { return leaf_.first; }
    Value& value()
    { return leaf_.second; }
};

template <typename Key,
          typename Value,
          typename NodeType,
          typename BtreeType>
class PointerEle : Ele<Key, Value> {
private:
    std::pair<Key, Node<Key, Value, NodeType, BtreeType>> leaf_;
public:
    Key& key() override
    { return leaf_.first; }
    Node<Key, Value, NodeType, BtreeType>& value()
    { return leaf_.second; }
};



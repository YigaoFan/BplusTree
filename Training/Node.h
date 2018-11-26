#pragma once
#ifndef NODE_H
#define NODE_H

#include <vector>
using std::vector;
typedef int ValueType;
class Node;
typedef bool(*PredicateFunc)(const Node&);

struct Ele {
    ValueType childValueLowBound = 1;
    Node* child = nullptr;
public:
    Ele() = default;
};

class Node {
public:
    Node();
    ~Node();
    vector<Ele>& getVectorOfEles();
    Node* giveMeTheWay(PredicateFunc func);

private:
    vector<Ele> es();
    // the value below could be customize
    // in the future
    vector<Ele> eles{(3)};
    Node* nextBrother;
    decltype(eles.size()) elesCount;
};

#endif

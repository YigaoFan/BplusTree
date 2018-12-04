#pragma once
#include <memory>
using std::shared_ptr;
extern class Node;

typedef int KeyType;
typedef char DataType;

struct Ele {
	// start from 1, ensure not initialization 0 situation
	enum Tag { intermediate_node = 1, leaf, } type;
	union {
		struct {
			// todo: the key type should provide a default value
			KeyType childValueLowBound;
			shared_ptr<Node> child;
		} /*intermediate_node*/;
		struct {
			// todo: the key and data type should provide a default value
			KeyType key;
			DataType data;
		} /*leaf_node*/;
	} /*u*/;

	Ele(KeyType key) : childValueLowBound(key), child(nullptr) {}
	Ele(DataType data) : key(data), data(data) {}
	Ele(const Ele& e);
	Ele& operator=(const Ele& e);
	~Ele();
};

